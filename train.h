#ifndef TRAIN_H
#define TRAIN_H

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

class Train {                                                       //Creating class and initializing its attributes

private:

    string trainName;

    string route;

    string schedule;

public:

    Train(string, string, string);

    virtual void getTrainDetails() const;

    virtual void updateSchedule(string);

    virtual ~Train() = default;
};

class LocalTrain : public Train {                                   //Creating derived class and initializing its attributes

public:

    LocalTrain(string, string, string);

    void display() const;
};

class ExpressTrain : public Train {                                 //Creating derived class and initializing its attributes

public:

    ExpressTrain(string, string, string);

    void display() const;
};

class TrainSeat {                                                   //Creating class and initializing its attributes

    string seatNumber;

    bool isBooked;

public:

    TrainSeat(string);

    bool isAvailable() const;

    void reserveSeat();

    void cancelSeat();

    string getSeatNumber() const;
};

class RouteInformation {                                            //Creating class and initializing its attributes

    string startLocation;

    string destination;

    double distance;

    string estimatedTime;

public:

    RouteInformation(string, string, double, string);

    void getRouteDetails() const;
};

class Passenger {                                                   // Simple association class

    string name;

    int age;

public:
    Passenger(string name, int age) : name(name), age(age) {}

    void displayInfo() const {
        cout << "Passenger Name: " << name << ", Age: " << age << endl;
    }
};

class TrainBooking {                                                //Creating class and initializing its attributes

    string trainNumber;

    double price;

    Train* train;                                                   // Aggregation

    vector<TrainSeat> seats;                                        // Composition

    map<int, string> seatStatusMap;

    vector<Passenger> passengers;                                   // Association

public:

    int totalSeats;

    int bookedSeats;

    TrainBooking(string, double, int, Train*);

    bool checkAvailability() const;

    double getPrice() const;

    void bookTicket(int);

    void cancelTicket(int);

    void viewTrainBooking() const;

    void saveToFile();

    static TrainBooking loadFromFile(string);

    void viewSeatStatus() const;

    void addPassenger(const Passenger& p);

    void viewPassengers() const;
};

class SystemInfo {                                                  //Creating class and initializing its attributes

private:

    static SystemInfo* instance;

    string version;

    string adminEmail;

    int totalTrains;

    SystemInfo();                                                   // Private constructor

public:

    static SystemInfo* getInstance();

    void showInfo() const;

    void setTotalTrains(int);
};

void trainSystemMenu(bool isAdmin);

#endif //TRAIN_H