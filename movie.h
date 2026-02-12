#ifndef MOVIE_H
#define MOVIE_H

#include <iostream>
#include <vector>
#include "json.hpp"
using json = nlohmann::json;
using namespace std;

class Cinema; //forward declaration of class here



////////////BASE CLASS OF ACCOUNT////////////

class Account {  //This class will be used for login purpose 
protected:
    string name, CNIC, password, userRole;
public:
    Account(string name_, string CNIC_, string password_, string role_);  //Parameterized constructor
    virtual ~Account();  //Destructor
    virtual void show();  //Using virtual function so it will be over ride in the derieve classes 
    //Setters & getters
    string getName()const;
    string getCNIC()const;
    string getPassword()const;
    string getUserRole()const;
    void resetPassword(const string& newPass);
    virtual json toJson()const;   //Virtual function that store data to json file
};

////////////DERIEVE CLASS OF ADMIN////////////

class Adminm : public Account {  //Derive class that injerit from parent class Account
public:
    Adminm(string name_, string CNIC_, string password_);    //Parameterized constructor
    Adminm(); //Default constructor
    void show()override;   //overriding the function from base class
};

////////////DERIEVE CLASS OF CUSTOMER////////////

class Customer : public Account {  //Derive class that injerit from parent class Account
public:
    Customer(string name_, string CNIC_, string password_);  //Parameterized constructor
    Customer();  //Default constructor
    void show() override;   //overriding the function from base class
};

////////////MOVIE CLASS ////////////

class Movie {  //Making movie class which maintain movie records 
public:
    string title, genre, movieSchedule;
    int duration, price;
    float rating;

    Movie();  //Default constructor
    Movie(string t, string g, int d, int p, string ms, float r=0.0f);     //Parameterized constructor
    json toJson()const;  
    static Movie fromJson(const json&);
    void display()const;
};

////////////MOVIE MANAGER CLASS ////////////

//Singleton class to Manage movies 
class MovieManager {      
private:
    vector<Movie> movies;   
    string filename;
    MovieManager();
public:

    int getMovieCount() const;
    static MovieManager& getInstance();
    void loadFromFile();
    void saveToFile();
    void addMovie();
    void removeMovie();
    void rateMovie();
    void viewMovies() const;
    float findMoviePriceByTitle(const string& title);
};

////////////BOOKING MANAGER CLASS ////////////

class BookingManager {
private:
    vector<json> bookings;
    string filename;
public:
    BookingManager();
    int getBookingCount() const;
    void addBooking(const json& b);
    void loadFromFile();
    void saveToFile();
    void viewByPhoneNo(const string& phone);
    void cancelByPhoneNo(const string& phone);
};

////////////CUSTOMER BOOKING CLASS ////////////

class CustomerBooking {
private:
    string phone;
    BookingManager& bookingManager;
public:
    CustomerBooking(string phone_, BookingManager&);
    void bookMovie(MovieManager& mg);
    void viewBookings();
    void cancelBookings();
};


////////////ACCOUNT MANAGER CLASS ////////////

class AccountManager {
private:
    vector<Account*> users;
    string filename;
    Account* findUser(const string& username, const string& role);
public:
    AccountManager();
    ~AccountManager();
    void loadFromFile();
    void saveToFile();
    void registerUser(string role);
    void loginUser(string role, Cinema& cinemaObj);
    void forgotPassword(string role);
};

////////////CINEMA CLASS ////////////

class Cinema {
public:
    void mainMenu();
    void adminLoginPage();
    void customerLoginPage();
    void adminMenu();
    void customerMenu();
    void colorChange();
    void setColor(int color);
};

////////////LIST OF MOVIE GENRES CLASS ////////////

template<typename T>   //Using template class here
class ListofMovieGenres{
    vector<T> items;
public:
    void add(const T& item){   //This function will add genres to the list
         items.push_back(item); 
        }
    void display()const{
        if (items.empty()) {
            cout << "List is empty.\n";
            return;
        }
        cout << "Items:\n";
        for (size_t i = 0; i<items.size(); ++i) {
            cout<<i+1<< ". "<<items[i]<<"\n";
        }
    }
};

////////////THREATER CLASS ////////////

class Theater {
    MovieManager& movieManager;     //Using Aggregation by reference here
    BookingManager& bookingManager;  //Using Aggregation by reference here
public:
    Theater(MovieManager& mm, BookingManager& bm) : movieManager(mm), bookingManager(bm){}   //Parameterized constructor
    void showStats(){
        cout << "Total Movies: " << movieManager.getMovieCount() << endl;
        cout << "Total Bookings: " << bookingManager.getBookingCount() << endl;
    }
};

#endif