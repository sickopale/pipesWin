#include "windows.h"
#include <iostream>
#include "string"

const unsigned int Size = 50;

std::string parse(std::string input){
    std::string output;
    while (!input.empty()) {
        auto end = find(input.begin(), input.end(), ' ');
        std::string tmp = input.substr(0, end - input.begin());
        if (end == input.end()) break;
        end++;
        input.erase(input.begin(), end);

        int num = stoi(tmp, nullptr, 10);
        output.append(std::to_string(num + 10) + " ");
    }
    return output;
}


int main(){

    HANDLE pipe2;
    while((pipe2 = CreateFile("\\\\.\\pipe\\$Pipe2$",PIPE_ACCESS_INBOUND,
                              FILE_SHARE_READ | FILE_SHARE_WRITE,nullptr,
                              OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr))
          == INVALID_HANDLE_VALUE){
    }

    if (pipe2 == INVALID_HANDLE_VALUE) {
        std::wcout << "A: Failed to connect to pipe." << std::endl;
        system("pause");
        return 1;
    }

    char buffer[Size];
    if (!ReadFile(pipe2,buffer,Size,
                  nullptr,nullptr)) {
        std::cout << "A: reading failed!\n";
    }

    std::string str(buffer);
    std::cout << "A got: " << str << "\n";
    std::string input = parse(str);
    std::cout << "A sent: " << input << "\n";

    HANDLE pipe3 = CreateNamedPipe(
            "\\\\.\\pipe\\$Pipe3$",PIPE_ACCESS_OUTBOUND,PIPE_TYPE_BYTE,
            1,Size,Size,0,nullptr);

    if (pipe3 == nullptr || pipe3 == INVALID_HANDLE_VALUE) {
        std::wcout << "A: Failed to create outbound pipe instance.\n";
        system("pause");
        return 1;
    }

    ConnectNamedPipe(pipe3, nullptr);

    if (!WriteFile(pipe3, input.c_str(), strlen(buffer),
                   nullptr, nullptr)){
        std::cout << "A: writing failed!\n";
    }

    CloseHandle(pipe2);
    CloseHandle(pipe3);

    system("pause");
    return 0;
}
