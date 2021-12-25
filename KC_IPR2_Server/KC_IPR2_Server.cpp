// KC_IPR2_Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#pragma comment(lib, "ws2_32.lib")
#include <iostream>
#include <vector>
//#include <sys/types.h>
//
//#include <clocale>
//#include <iostream>
//#include <Windows.h>
//#include <sys/types.h>
//#include <string.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <signal.h>
//#include <winsock2.h>
//using namespace std;

#include <stdio.h>
#include <winsock2.h>
using namespace std;

//Создаем массив структор для хранения информации о списке книг
struct book_data {
	char registration_number[40];  // регистрационный номер книги
	char author[40];  // автор 
	char name[40]; // название
	char year_published[40];  // год издания 
	char publisher[40]; //издательство
	char quantity_page[40]; //количество страниц
};

void feel_data(vector<book_data>& books) {
	book_data book;

	strcpy_s(book.registration_number, "ISBN-10 111");
	strcpy_s(book.author, "Pushkin");
	strcpy_s(book.name, "Ruslan i Ludmila");
	strcpy_s(book.year_published, "2021");
	strcpy_s(book.publisher, "Belyi dom");
	strcpy_s(book.quantity_page, "630");
	books.push_back(book);

	strcpy_s(book.registration_number, "ISBN-10 112");
	strcpy_s(book.author, "Pushkin");
	strcpy_s(book.name, "Dubrovskiy");
	strcpy_s(book.year_published, "2020");
	strcpy_s(book.publisher, "Belyi dom");
	strcpy_s(book.quantity_page, "201");
	books.push_back(book);

	strcpy_s(book.registration_number, "ISBN-10 113");
	strcpy_s(book.author, "Esenin");
	strcpy_s(book.name, "Pismo k gencine");
	strcpy_s(book.year_published, "2019");
	strcpy_s(book.publisher, "Skaz");
	strcpy_s(book.quantity_page, "373");
	books.push_back(book);

	strcpy_s(book.registration_number, "ISBN-10 114");
	strcpy_s(book.author, "Esenin");
	strcpy_s(book.name, "Chernyi chelovek");
	strcpy_s(book.year_published, "2000");
	strcpy_s(book.publisher, "Skaz");
	strcpy_s(book.quantity_page, "100");
	books.push_back(book);

	strcpy_s(book.registration_number, "ISBN-10 115");
	strcpy_s(book.author, "Bulgakov");
	strcpy_s(book.name, "Master i Margarita");
	strcpy_s(book.year_published, "1999");
	strcpy_s(book.publisher, "Skaz");
	strcpy_s(book.quantity_page, "500");
	books.push_back(book);
}

void print_books(vector<book_data>& books) {
	for (int i = 0; i < size(books); i++) {
		cout << books[i].registration_number << endl;
		cout << books[i].author << endl;
		cout << books[i].name << endl;
		cout << books[i].year_published << " " << books[i].quantity_page << endl;
		cout << "===================" << endl;
	}
}

DWORD WINAPI ThreadFunc(LPVOID client_socket) {
	SOCKET s2 = ((SOCKET*)client_socket)[0];
	char buf[100];
	char buf1[100];
	//send(s2,"Welcome new client!\n",sizeof("Welcome new client!\n"),0);
	while (recv(s2, buf, sizeof(buf), 0)) {
		cout << "Server received command: "<< buf << endl;
		send(s2, buf, 100, 0);
	}
	closesocket(s2);
	return 0;
}

void printConnectionIsReady(int& clientNumber) {
	if (clientNumber) printf("%d client connected\n", clientNumber);
	else printf("No clients connected\n");
}

int main() {
	//vector<book_data> books_list;
	//feel_data(books_list);
	//print_books(books_list);
	//cout << "Size of data: " << size(books_list) << endl;

	int clientNumber = 0;

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) { return 0; }
	SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(1280);
	local_addr.sin_addr.s_addr = 0;
	bind(s, (sockaddr*)&local_addr, sizeof(local_addr));
	int c = listen(s, 5);
	cout << "Server receive ready" << endl;
	cout << endl;
	// извлекаем сообщение из очереди
	SOCKET client_socket; // сокет для клиента
	sockaddr_in client_addr; // адрес клиента (заполняется системой)
	int client_addr_size = sizeof(client_addr);
	// цикл извлечения запросов на подключение из очереди
	while ((client_socket = accept(s, (sockaddr*)&client_addr,
		&client_addr_size))) {
		clientNumber++;
		printConnectionIsReady(clientNumber);
		// Вызов нового потока для обслуживания клиента
		DWORD thID;// thID идентификатор типа DWORD
		CreateThread(NULL, NULL, ThreadFunc, &client_socket, NULL, &thID);
	}
	return 0;
}
