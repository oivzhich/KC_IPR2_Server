#pragma comment(lib, "ws2_32.lib")
#include <iostream>
#include <vector>

#include <stdio.h>
#include <winsock2.h>
#include <string>
#include <algorithm>
#include <format>
using namespace std;

//Создаем массив структор для хранения информации о списке книг
struct book_data {
	string registration_number;  // регистрационный номер книги
	string author;  // автор 
	string name; // название
	int year_published;  // год издания 
	string publisher; //издательство
	int quantity_page; //количество страниц
};

vector<book_data> books_list;

void feel_data(vector<book_data>& books) {
	book_data book;

	book.registration_number = "ISBN-10 111";
	book.author = "Pushkin";
	book.name = "Ruslan i Ludmila";
	book.year_published = 2021;
	book.publisher = "Belyi dom";
	book.quantity_page = 630;
	books.push_back(book);

	book.registration_number = "ISBN-10 112";
	book.author = "Pushkin";
	book.name = "Dubrovskiy";
	book.year_published = 2020;
	book.publisher = "Belyi dom";
	book.quantity_page = 201;
	books.push_back(book);

	book.registration_number = "ISBN-10 113";
	book.author = "Esenin";
	book.name = "Pismo k gencine";
	book.year_published = 2019;
	book.publisher = "Skaz";
	book.quantity_page = 373;
	books.push_back(book);

	book.registration_number = "ISBN-10 114";
	book.author = "Esenin";
	book.name = "Chernyi chelovek";
	book.year_published = 2000;
	book.publisher = "Skaz";
	book.quantity_page = 100;
	books.push_back(book);

	book.registration_number = "ISBN-10 115";
	book.author = "Bulgakov";
	book.name = "Master i Margarita";
	book.year_published = 1999;
	book.publisher = "Skaz";
	book.quantity_page = 500;
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

string get_all_books() {
	string message;
	for (book_data book : books_list) {
		string str = "[" + book.registration_number + "] " + book.author + ": " + book.name + ", " + book.publisher +
			", " + to_string(book.year_published) + " - " + to_string(book.quantity_page) + " c\n";
		message += str;
	}
	return message;
}

string deleteBook(const char* buf) {
	string message("Will delete book");
	int book_index = -1;
	try {
		book_index = stoi(buf);
	}
	catch (const std::invalid_argument& e) {
		std::cout << e.what() << "\n";
	}
	catch (const std::out_of_range& e) {
		std::cout << e.what() << "\n";
	}
	cout << "Recieved book index to delete: " << book_index << endl;
	if (book_index < 0) {
		message = "Incorrect input for book index";
	}
	else {
		if (books_list.size() < book_index) {
			message = "Provided book index does not exist";
		}
		else {
			books_list.erase(books_list.begin() + book_index);
			message = "The book with index " + to_string(book_index) + " has been deleted";
		}
	}
	return message;
}

int bufToInt(const char buf) {
	return -1;
}

DWORD WINAPI ThreadFunc(LPVOID client_socket) {
	char main_menu_choise, // yein
		switch_menu_choise,	//yes
		buf[500],
		result_quantity[30]; //yes

	int num = 0;

	SOCKET socket = ((SOCKET*)client_socket)[0];

	while (true) {
		memset(&buf[0], 0, sizeof(buf));
		//Получаем команду от клиента
		recv(socket, buf, sizeof(buf), 0);
		main_menu_choise = buf[0];
		int books_size;
		vector<book_data> books_filtered;
		string author_name = "";
		string result_books_names;
		string message;

		cout << "Server received command: " << buf << endl;

		switch (main_menu_choise) {
		case '1':
			memset(&buf[0], 0, sizeof(buf));
			//получаем имя автора
			recv(socket, buf, sizeof(buf), 0);
			buf[strcspn(buf, "\n")] = 0;
			author_name = buf;

			cout << "Recieved author name: " << author_name << endl;

			copy_if(books_list.begin(), books_list.end(), back_inserter(books_filtered),
				[&author_name](auto const& book)
				{ return book.author == author_name; });

			if (books_filtered.size() == 0) {
				message = "No author found. Please try to enter different author";
			}
			else {
				for (unsigned i = 0; i < books_filtered.size(); i++) {
					cout << books_filtered[i].name << endl;
					message += books_filtered[i].name + "\n";
				}
			}

			//отправляем результат поиска
			send(socket, message.c_str(), message.size(), 0);
			break;
		case '2':
			if (books_list.size() > 0) {
				message = get_all_books();
			}
			else {
				message = "No author found. Please try to enter different author";
			}
			//отправляем результат
			send(socket, message.c_str(), message.size(), 0);
			break;

		case '4':
			memset(&buf[0], 0, sizeof(buf));
			//получаем имя автора
			recv(socket, buf, sizeof(buf), 0);
			buf[strcspn(buf, "\n")] = 0;
			message = deleteBook(buf);

			//memset(&buf[0], 0, sizeof(buf));
			////получаем индекс книги для удаления
			//recv(socket, buf, sizeof(buf), 0);
			//message = deleteBook(buf);

			//отправляем результат удаления
			send(socket, message.c_str(), message.size(), 0);
			break;
		default:
			cout << "Coonection will be closed" << endl;
			closesocket(socket);
			exit(0);
			break;
		}
	}
	return 0;
}

void printConnectionIsReady(int& clientNumber) {
	if (clientNumber) printf("%d client connected\n", clientNumber);
	else printf("No clients connected\n");
}

int main() {
	feel_data(books_list);
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
