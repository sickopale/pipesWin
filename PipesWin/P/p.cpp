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
        output.append(std::to_string(num*num*num) + " ");
    }
    return output;
}

int main() {
    HANDLE pipe1 = CreateFile(
            "\\\\.\\pipe\\$Pipe1$",
            PIPE_ACCESS_INBOUND,
            FILE_SHARE_READ,
            nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            nullptr
    );

    if (pipe1 == INVALID_HANDLE_VALUE) {
        std::wcout << "P: Failed to connect to pipe." << std::endl;
        system("pause");
        return 1;
    }

    char buffer[Size];
    DWORD numBytesRead = 0;
    if (!ReadFile(pipe1,buffer,Size,
                  &numBytesRead,nullptr)) {
        std::cout << "P: reading failed!\n";
    }

    HANDLE pipe2 = CreateNamedPipe(
            "\\\\.\\pipe\\$Pipe2$",PIPE_ACCESS_OUTBOUND,PIPE_TYPE_BYTE,
            1,Size,Size,0,nullptr);

    if (pipe2 == nullptr || pipe2 == INVALID_HANDLE_VALUE) {
        std::wcout << "P: Failed to create outbound pipe instance.\n";
        system("pause");
        return 1;
    }

    ConnectNamedPipe(pipe2, nullptr);

    DWORD numBytesWritten = 0;
    std::string str(buffer);
    std::cout << "P got: " << str << "\n";
    std::string input = parse(str);
    std::cout << "P sent: " << input << "\n";

    WriteFile(pipe2, input.c_str(), strlen(buffer),
              &numBytesWritten, nullptr);

    CloseHandle(pipe1);
    CloseHandle(pipe2);

    system("pause");
    return 0;
}
