#include <iostream>
#include <iomanip>
#include <cstring>
#include <ctime>
#include <cstdlib>
using namespace std;

enum BookDataSelector {
    PUBLISHING,
    TITLE,
    AUTHOR,
    PRICE
};

struct Book {
    char* id;
    char* publishing;
    char* title;
    char* author;
    int price;

    Book(): id(nullptr), publishing(nullptr), title(nullptr), author(nullptr), price(0) {}
};

struct BookShop {
    char* name;
    char* director;
    Book* books_list;
    int books_list_size;
    int position;

    BookShop(): name(nullptr), director(nullptr), position(0), books_list(nullptr), books_list_size(0) {}
};


#pragma region ServiceFunctions

char* copyString(const char* source) {
    if (source == nullptr) return nullptr;
    int len = strlen(source);
    char* copy = new char[len + 1];
    strcpy(copy, source);
    return copy;
}

void freeString(char* str) {
    if (str != nullptr) {
        delete[] str;
    }
}

bool isValidString(const char* str, int minLength = 3) {
    return str != nullptr && strlen(str) >= minLength;
}

void generateID(char* buffer) {
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    const int charsetSize = sizeof(charset) - 1;
    const int length = 10;

    for (int i = 0; i < length; ++i) {
        buffer[i] = charset[rand() % charsetSize];
    }
    buffer[length] = '\0';
}

#pragma endregion

#pragma region BookFunctions

void cleanUpBook(Book& book) {
    freeString(book.id);
    freeString(book.publishing);
    freeString(book.title);
    freeString(book.author);

    book.id = nullptr;
    book.publishing = nullptr;
    book.title = nullptr;
    book.author = nullptr;
    book.price = 0;
}

void fillBookInfo(Book& book, const char* publishing, const char* title, const char* author, int price) {
    if (!isValidString(publishing) || !isValidString(title) || !isValidString(author) || price <= 0) {
        cout << "Invalid book info\n";
        return;
    }

    cleanUpBook(book);

    char tempID[11];
    generateID(tempID);
    book.id = copyString(tempID);
    book.publishing = copyString(publishing);
    book.title = copyString(title);
    book.author = copyString(author);
    book.price = price;
}

void changeBookData(Book& book, BookDataSelector field_selector, const char* new_value) {
    if (!isValidString(new_value)) {
        cout << "Invalid new value\n";
        return;
    }

    char** selected_field = nullptr;
    switch (field_selector) {
        case PUBLISHING: selected_field = &book.publishing; break;
        case TITLE: selected_field = &book.title; break;
        case AUTHOR: selected_field = &book.author; break;
        default:
            cout << "Invalid field selected\n";
            return;
    }

    freeString(*selected_field);
    *selected_field = copyString(new_value);
}

void changeBookData(Book& book, BookDataSelector field_selector, int new_value) {
    if (field_selector == PRICE && new_value > 0) {
        book.price = new_value;
    } else {
        cout << "Invalid field or value\n";
    }
}

Book copyBook(const Book& source) {
    Book copy;
    if (source.id != nullptr) {
        fillBookInfo(copy, source.publishing, source.title, source.author, source.price);
        freeString(copy.id);
        copy.id = copyString(source.id);
    }
    return copy;
}

#pragma endregion

#pragma region BookShopFunctions

BookShop createBookShop(const char* name, const char* director) {
    BookShop book_shop;

    if (!isValidString(name) || !isValidString(director)) {
        cout << "Invalid book shop info\n";
        return book_shop;
    }

    book_shop.name = copyString(name);
    book_shop.director = copyString(director);

    return book_shop;
}

void cleanUpBookShop(BookShop& book_shop) {
    if (book_shop.books_list != nullptr) {
        for (int i = 0; i < book_shop.position; i++) {
            cleanUpBook(book_shop.books_list[i]);
        }
        delete[] book_shop.books_list;
    }

    freeString(book_shop.name);
    freeString(book_shop.director);

    book_shop.name = nullptr;
    book_shop.director = nullptr;
    book_shop.books_list = nullptr;
    book_shop.books_list_size = 0;
    book_shop.position = 0;
}

void addBookToLibrary(const Book& book, BookShop& book_shop) {
    if (book_shop.books_list == nullptr) {
        book_shop.books_list_size = 4;
        book_shop.books_list = new Book[book_shop.books_list_size];
        book_shop.position = 0;
    }
    else if (book_shop.position >= book_shop.books_list_size) {
        Book* temp = new Book[book_shop.books_list_size * 2];
        for (int i = 0; i < book_shop.position; i++) {
            temp[i] = book_shop.books_list[i];
        }
        delete[] book_shop.books_list;
        book_shop.books_list = temp;
        book_shop.books_list_size *= 2;
    }

    book_shop.books_list[book_shop.position] = copyBook(book);
    book_shop.position++;
}

void deleteBookFromLibrary(const Book& book, BookShop& book_shop) {
    if (book.id == nullptr || book_shop.books_list == nullptr) return;

    for (int i = 0; i < book_shop.position; i++) {
        if (book_shop.books_list[i].id != nullptr && strcmp(book_shop.books_list[i].id, book.id) == 0) {
            cleanUpBook(book_shop.books_list[i]);
            for (int j = i; j < book_shop.position - 1; j++) {
                book_shop.books_list[j] = book_shop.books_list[j + 1];
            }
            book_shop.position--;
            break;
        }
    }
}

BookShop* searchInPriceRange(const BookShop& book_shop, int min_price, int max_price) {
    if (book_shop.books_list == nullptr || min_price > max_price) return nullptr;

    int result_count = 0;
    for (int i = 0; i < book_shop.position; i++) {
        if (book_shop.books_list[i].price >= min_price && book_shop.books_list[i].price <= max_price) {
            result_count++;
        }
    }

    if (result_count == 0) return nullptr;

    BookShop* result_shop = new BookShop();
    result_shop->name = copyString(book_shop.name);
    result_shop->director = copyString(book_shop.director);
    result_shop->books_list = new Book[result_count];
    result_shop->books_list_size = result_count;
    result_shop->position = 0;

    for (int i = 0; i < book_shop.position; i++) {
        if (book_shop.books_list[i].price >= min_price && book_shop.books_list[i].price <= max_price) {
            result_shop->books_list[result_shop->position] = copyBook(book_shop.books_list[i]);
            result_shop->position++;
        }
    }

    return result_shop;
}

void sortLibrary(BookShop& book_shop) {
    if (book_shop.books_list == nullptr || book_shop.position <= 1) return;

    for (int i = 0; i < book_shop.position - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < book_shop.position; j++) {
            if (book_shop.books_list[j].price < book_shop.books_list[min_idx].price) {
                min_idx = j;
            }
        }
        if (min_idx != i) {
            Book temp = book_shop.books_list[i];
            book_shop.books_list[i] = book_shop.books_list[min_idx];
            book_shop.books_list[min_idx] = temp;
        }
    }
}

int countBooksInLibrary(const BookShop& book_shop) {
    return book_shop.position;
}

void printLibrary(const BookShop& book_shop) {
    if (book_shop.name == nullptr || book_shop.director == nullptr) {
        cout << "Invalid bookshop\n";
        return;
    }

    cout << "\n" << book_shop.name << " | " << book_shop.director << "\n";
    cout << string(95, '-') << "\n";
    cout << left
         << setw(15) << "ID"
         << "| " << setw(12) << "PUBLISHER"
         << "| " << setw(25) << "TITLE"
         << "| " << setw(20) << "AUTHOR"
         << "| " << setw(11) << "PRICE (UAH)"
         << "|\n";
    cout << string(95, '-') << "\n";

    for (int i = 0; i < book_shop.position; i++) {
        const Book& book = book_shop.books_list[i];
        cout << left
             << setw(15) << (book.id ? book.id : "N/A")
             << "| " << setw(12) << (book.publishing ? book.publishing : "N/A")
             << "| " << setw(25) << (book.title ? book.title : "N/A")
             << "| " << setw(20) << (book.author ? book.author : "N/A")
             << "| " << setw(11) << book.price
             << "|\n";
    }
    cout << string(95, '-') << "\n";
}

#pragma endregion

int main() {
    srand(time(nullptr));

    Book book_1, book_2, book_3, book_4, book_5;
    fillBookInfo(book_1, "KCD", "Dune", "Frank Herbert", 654);
    fillBookInfo(book_2, "KCD", "Children Of Dune", "Frank Herbert", 623);
    fillBookInfo(book_3, "KCD", "Game Of Thrones", "George Martin", 234);
    fillBookInfo(book_4, "KCD", "Hunger Games", "Artem Posmitiukha", 324);
    fillBookInfo(book_5, "KCD", "Soyuz Corp Documentation", "Henadii Posmitiukha", 1000);

    BookShop book_shop = createBookShop("IT STEP", "Keba Evhenii");
    addBookToLibrary(book_1, book_shop);
    addBookToLibrary(book_2, book_shop);
    addBookToLibrary(book_3, book_shop);
    addBookToLibrary(book_4, book_shop);
    addBookToLibrary(book_5, book_shop);

    cout << "Original library:";
    printLibrary(book_shop);

    cout << "\nAfter deleting first book:";
    deleteBookFromLibrary(book_1, book_shop);
    printLibrary(book_shop);

    cout << "\nBooks in price range 1-600:";
    BookShop* books_in_range = searchInPriceRange(book_shop, 1, 600);
    if (books_in_range != nullptr) {
        printLibrary(*books_in_range);
        cleanUpBookShop(*books_in_range);
        delete books_in_range;
    } else {
        cout << "No books found in range\n";
    }

    cout << "\nAfter sorting by price:";
    sortLibrary(book_shop);
    printLibrary(book_shop);

    cout << "\nTotal books: " << countBooksInLibrary(book_shop) << "\n";

    cleanUpBook(book_1);
    cleanUpBook(book_2);
    cleanUpBook(book_3);
    cleanUpBook(book_4);
    cleanUpBook(book_5);
    cleanUpBookShop(book_shop);

    return 0;
}

