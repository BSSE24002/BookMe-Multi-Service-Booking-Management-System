#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include "json.hpp"
#include <iomanip>
#include <ctime>
#include <chrono>
#include<sstream>
using json=nlohmann::json;

using namespace std;

//================================================ SCHEDULE CLASS================================================

class Schedule {
    string arrivalTime;
    string departureTime;
    string date;
    public:
    Schedule(string departureTime,string arrivalTime ,string date ="TBD") {
        this->arrivalTime = arrivalTime;
        this->departureTime = departureTime;
        this->date = date;
    }
    void setSchedule(string departureTime, string arrivalTime,string date) {
        this->arrivalTime = arrivalTime;
        this->departureTime = departureTime;
        this->date = date;
    }
    string getArrivalTime() const {
        return this->arrivalTime;
    }
    string getDepartureTime() const {
        return this->departureTime;
    }
    string getDate() const {
        return this->date;
    }
    void displaySchedule() const {
        cout << "Date: "<<date<<" | Departure Time: " << this->departureTime <<" | Arrival Time: "<<arrivalTime<< endl;
    }
};

//===================================== AIRLINE CLASS (Aggregation with Flight)==================================

class Airline {
private:
    string airlineName;
public:
    Airline(string name) : airlineName(name) {}
    void setAirlineName(string name) ;
    string getAirlineName() const { return airlineName; }
};

// ==================================================BASE FLIGHT CLASS=======================================

class Flight {
protected:
    string flightID, from, destination;
    double price;
    int totalSeats, bookedSeats;
    int lastBookedSeats;
    Schedule schedule;
public:
    Airline* airline; // Aggregation

    Flight(string id, string f, string d, double p, int seats, Airline* air ,string depTime="00:00",string arrival="00:00", string date="TBD")
        : flightID(id), from(f), destination(d), price(p), totalSeats(seats), bookedSeats(0), airline(air) ,schedule(depTime, arrival,date) {}

    string getFlightID() const { return flightID; }
    string getFrom() const { return from; }
    double getPrice() const { return price; }
    int getTotalSeats() const { return totalSeats; }
    int getBookedSeats() const { return bookedSeats; }
    Airline* getAirline() const { return airline; }
    string getDestination() const { return destination; }
    virtual void display() const ;
    bool bookSeat(int num);
    void setLastBookedSeats(int num){lastBookedSeats=num;}
    int getLastBookedSeats() const { return lastBookedSeats; }
    void cancelSeat(int num) ;
    void setSchedule(string dep,string arr,string date){schedule.setSchedule(arr,dep,date);}
    Schedule getSchedule() const{return schedule;}

    friend ostream& operator<<(ostream& os, const Flight& flight);


};

//=============================== Derived Classes for Domestic and International Flights==============================

class DomesticFlight : public Flight {
public:
    DomesticFlight(string id, string f, string d, double p, int seats, Airline* air,string dep,string arr,string date)
        : Flight(id, f, d, p, seats, air,dep,arr,date) {}
    void display() const override {
        cout << "[Domestic] ";
        Flight::display();
    }
};

class InternationalFlight : public Flight {
public:
    InternationalFlight(string id, string f, string d, double p, int seats, Airline* air,string dep ,string arr,string date)
        : Flight(id, f, d, p, seats, air,dep,arr,date) {}
    void display() const override {
        cout << "[International] ";
        Flight::display();
    }
};

//================================================PAYMENT RECORD CLASS============================================

struct PaymentRecord {
    string flightID;
    string airlineName;
    double amount;
    string date;
    string method;

    static string getCurrentDate();

    json toJson() const ;

    static PaymentRecord fromJson(const json& j) ;
};

//==================================== FLIGHT BOOKING SYSTEM CLASS ==============================================

class FlightBooking {
private:
    static FlightBooking* instance;        // Step 1: static instance
    FlightBooking() {}                     // Step 2: private constructor

    // Optional: to prevent copying/cloning
    FlightBooking(const FlightBooking&) = delete;
    FlightBooking& operator=(const FlightBooking&) = delete;
    vector<Flight*> flights;
    vector<Airline*> airlines;
    map<string, vector<PaymentRecord>> airlinePaymentDetails;
public:
    static FlightBooking* getInstance() {  // Step 3: getInstance
        if (!instance)
            instance = new FlightBooking();
        return instance;
    }
    ~FlightBooking() {
        for (Flight* f : flights) delete f;
        for (Airline* a : airlines) delete a;

    }
    void addAirline(string name) ;
    void saveToFile(string filename);
    void loadFromFile(string filename);
    void addFlight(string id, string from, string to, double price, int seats, string airlineName, string type,string dep,string arr,string date) ;
    void addFlight(string id, string from, string to, double price, int seats, string airlineName) ;

    void viewFlights() const ;
    Flight* findFlight(string flightID) ;
   // Flight *findFlight(string to, string from);

    void savePaymentsToFile(const std::string& filename);
    void loadPaymentsFromFile(const std::string& filename) ;
    void recordPayment(Flight* flight, double amount, string method) ;
    void displayPaymentsForAirline(const string& airlineName) const;
    vector<string> getAllAirlineNames() const;

};

//===================================================TEMPLATE CLASS===============================================

template <typename T>
class Payment {
    T amount;
    string method;
    public:
    Payment(T amount, string method) : amount(amount), method(method) {}
   T getAmount() const { return amount; }
    void processPayment() {
        cout << "Processing Payment...\n";
        cout << "Amount: $" << amount << " | Method: " << method << endl;
        cout << "Payment Successful!\n";
    }

};

//=================================================USER CLASS=======================================================

class User {
protected:
    string username, password;
    Payment<double>* payment;
public:
    User(string u, string p) : username(u), password(p), payment(nullptr) {}
    ~User() {
        delete payment;
    }
    bool login(string u, string p) const { return (username == u && password == p); }
    void accessFlights(FlightBooking& fb);
    bool makePayment(double amount,string& method);
};

// ==========================================ADMIN CLASS (Inherits from User)===================================

class Admin : public User {

public:
    Admin(string u, string p) : User(u, p ){}

    void manageFlights(FlightBooking& fb) ;
};
