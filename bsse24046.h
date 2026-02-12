#ifndef HOTEL_H
#define HOTEL_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include "json.hpp"
using namespace std;
using json = nlohmann::json;


class Room;
class Hotel;
class HotelBooking;

// ---------------- Friend Class ------------------
class HotelBooking {
    double price;
    int nights;
    Room* room;

public:
    HotelBooking(double pr = 0.0, int n = 0);
    void assignRoom(Room* r);
    bool cancelBooking();
    void display() const;
    double getPrice() const {
        return price*nights;
    }
    friend class Room;
    friend void to_json(json& j, const HotelBooking& b);
    friend void from_json(const json& j, HotelBooking& b);
};

// ---------------- Room Class ------------------
class Room {
    string roomNumber;
    string type;
    double price;
    bool isBooked;

public:
    bool getIsBooked() const { return isBooked; }
    Room(string num = "", string t = "Standard", double p = 100.0);
    bool isAvailable() const;
    void reserve();
    void cancel();
    string getRoomNumber() const;
    string getType() const { return type; }
    double getPrice() const { return price; }
    operator double() const {
        return price;
    }
    friend class HotelBooking;
};

// ---------------- Hotel Base Class ------------------
class Hotel {
protected:
    string hotelName;
    string location;
    int rating;
    vector<Room> rooms;

public:
    Hotel(string name = "", string loc = "", int r = 0);
    virtual void display() const;
    void addRoom(Room r);
    Room* findAvailableRoom();
    string getName() const;
    string getLocation() const;
    int getRating() const;
    vector<Room>& getRooms();  
    void editDetails(); 
    Hotel& operator+=(const Room& r) {
        addRoom(r);
        return *this;
    }
};

// ---------------- Inheritance ------------------
class LuxuryHotel : public Hotel {
public:
    LuxuryHotel(string name, string loc);
    void display() const override;
};

class StandardHotel : public Hotel {
public:
    StandardHotel(string name, string loc);
    void display() const override;
};

// ---------------- Singleton ------------------
class HotelManager {
    static HotelManager* instance;
    vector<Hotel*> hotels;

    HotelManager() {}

public:
    static HotelManager* getInstance();
    void addHotel(Hotel* h);
    vector<Hotel*>& getHotels();
};
// ---------------- User Class ------------------
class User_r {
    string username;
    string password;
    vector<HotelBooking> bookings;

public:
    User_r(string uname = "", string pass = "");
    bool checkPassword(const string& pass) const;
    void addBooking(const HotelBooking& b);
    const vector<HotelBooking>& getBookings() const;
    string getUsername() const;

    friend void to_json(json& j, const User_r& u);
    friend void from_json(const json& j, User_r& u);
};

template <typename T>
class TotalAmount {
private:
    T total;

public:
    TotalAmount() : total(0) {}
    TotalAmount(T value) : total(value) {}
    TotalAmount<T> operator+(const T& other) {
        TotalAmount<T> result;
        result.total = this->total + other;
        return result;
    }
    TotalAmount<T> operator-(const T& other) {
        TotalAmount<T> result;
        result.total = this->total - other;
        return result;
    }
    T getTotal() const {
        return total;
    }
    void display() const {
        cout << "Total Amount: PKR" << total << endl;
    }
};

void hotelBookingMenu(bool isAdmin);

#endif
