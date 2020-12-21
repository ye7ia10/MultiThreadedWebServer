#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <fstream>

using namespace std;
vector<string> getCommandsFromFile (string fileName);
vector<string> splitRequest(string str);
string getPostFileContent(string fileName);
void saveFile (string fileName, string content);



int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        cout << "Invalid Arguments" << endl;
        cout << flush;
        return -1;
    }

    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    int portNom = atoi(argv[2]);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        cout << endl << " Client Socket creation error " << endl;
        cout << flush;
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portNom);


    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        cout << "Address not supported " << endl;
        cout << flush;
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        cout << "Connection Failed" << endl;
        cout << flush;
        return -1;
    }

    vector<string> commands = getCommandsFromFile("request.txt");
    for (int i = 0; i < commands.size() ; i++){
        string command = commands[i];
        vector<string> splits = splitRequest(command);
        bzero(buffer, 1024);
        if (splits[0] == "get" || splits[0] == "client_get"){
            string type = splits[0];
            char * tempBuffer = &command[0];
            cout << endl << command << endl;
            cout << flush;
            send(sock, tempBuffer, strlen(tempBuffer), 0 );
            read(sock, buffer, 1024);
            cout << buffer << endl;
            cout << flush;
            int size;
            read(sock, &size, sizeof(int));
            cout << "String Size : " << size << endl;
            cout << flush;
            string content = "";
            while (true){
                char contentBuffer [1024];
                if (content.size() == size){
                    break;
                }
                long valRead = read(sock,contentBuffer,1024);
                if (valRead <= 0){
                    cout << "File Completed";
                    cout << flush;
                    break;
                }
                content += string(contentBuffer,valRead);
            }
            cout << content << endl;
            cout << flush;
            saveFile(splits[1] , content);
        } else if (splits[0] == "post" || splits[0] == "client_post"){
            string type = splits[0];
            char * tempBuffer = &command[0];
            cout << endl <<command << endl;
            cout << flush;
            send(sock, tempBuffer, strlen(tempBuffer), 0 );
            read(sock, buffer, 1024);
            cout << buffer << endl;
            cout << flush;
            string fileContent = getPostFileContent(splits[1]);
            char * tempFile = &fileContent[0];
            cout << fileContent << endl;
            cout << flush;
            send(sock, tempFile, strlen(tempFile), 0 );
            bzero(buffer, 1024);
            read(sock, buffer, 1024);
            cout << buffer << endl;
        }

    }

   while (1)
    {
        fgets(buffer, 1024,stdin);
        string req = buffer;
        send(sock, buffer, strlen(buffer), 0 );
        cout << "Buffer Sent" << endl;
        cout << flush;
        bzero(buffer, 1024);
        read(sock, buffer, 1024);
        cout << buffer << endl;
        cout << flush;
        bzero(buffer, 1024);
        vector<string> spl = splitRequest(req);
        if (spl[0] == "post" || spl[0] == "client_post"){
            string fileContent = getPostFileContent(spl[1]);
            char * tempFile = &fileContent[0];
            cout << fileContent << endl;
            cout << flush;
            send(sock, tempFile, strlen(tempFile), 0 );
            read(sock, buffer, 1024);
            cout << buffer << endl;
            cout << flush;
            bzero(buffer, 1024);
        } else if (spl[0] == "get" || spl[0] == "client_get"){
            int size;
            read(sock, &size, sizeof(int));
            cout << "String Size : " << size << endl;
            cout << flush;
            string content = "";
            while (true){
                char contentBuffer [1024];
                if (content.size() == size){
                    break;
                }
                long valRead = read(sock,contentBuffer,1024);
                if (valRead <= 0){
                    cout << "File Completed";
                    cout << flush;
                    break;
                }
                content += string(contentBuffer,valRead);
            }
            cout << content << endl;
            saveFile(spl[1] , content);
        }
    }
    return 0;
}

vector<string> getCommandsFromFile (string fileName)
{
    vector<string> commands;
    string line;
    string content = "";
    ifstream myfile;
    myfile.open(fileName);
    while(getline(myfile, line))
    {
        commands.push_back(line);
    }
    return commands;
}
vector<string> splitRequest(string str)
{
    vector<string> vec;
    int counter = 0;
    string word = "";
    for (auto x : str)
    {
        if (x == ' ')
        {
            vec.push_back(word);
            word = "";
            counter++;
            if (counter == 2)
            {
                break;
            }
        }
        else
        {
            if (x != '/')
            {
                word = word + x;
            }
        }

    }
    return vec;
}



string getPostFileContent(string fileName){
    string line;
    string content = "";
    ifstream myfile;
    myfile.open(fileName);
    while(getline(myfile, line))
    {
       content += line;
    }
    return content;
}



void saveFile (string fileName, string content){
    ofstream f_stream(fileName.c_str());
    f_stream.write(content.c_str(), content.length());
}

