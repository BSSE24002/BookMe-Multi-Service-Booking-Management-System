#ifndef CARRENTAL_H
#define CARRENTAL_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

// --- TravelRoute ---
class TravelRoute {
    string from;
    string to;
    string routePlan;
    double distanceKm;
    double baseRatePerKm;

  public:
    static map<string, TravelRoute> routeData;

    TravelRoute();
    TravelRoute(const string& from,
                const string& to,
                double dist,
                double rate);

    void generateRoute() ;
    void displayRoute() const;

    // JSON
    static void loadRoutesFromFile();
    static void saveRoutesToFile();
    string toJSON() const;

    // accessors
    string getFrom() const;
    string getTo() const;
    string getRoutePlan() const;
    double getEstimatedPrice() const;
    string getKey() const;

    static TravelRoute findRoute(const string& from,
                                 const string& to);
};

// --- Car ---
class Car {
    string model;
    string registration;
    bool available;
    double priceMultiplier;

  public:
    Car(string m, string reg, double multiplier, bool avail = true);

    string getModel() const;
    string getRegistration() const;
    bool isAvailable() const;
    double getPriceMultiplier() const;

    void markAvailable();
    void markUnavailable();
};

// --- CarBooking ---
class CarBooking {
    string username;
    Car car;
    TravelRoute route;
    double totalPrice;
    int bookingId;
    static int nextId;

  public:
    CarBooking() : username(""), car("DefaultModel", "XXX-000", 1.0, true), route(), totalPrice(0), bookingId(0) {}
    CarBooking(const string& user, const Car& c, const TravelRoute& r);

    int getId() const;
    const string& getUsername() const;
    const TravelRoute& getRoute() const;
    const Car& getCar() const;
    double getTotalPrice() const;

    void calculatePrice();
};

// --- CarManager ---
class CarManager {
    vector<Car> cars;

  public:
    void addCar(const Car& car);
    void removeCar(const string& reg);
    Car* findCarByReg(const string& reg);
    const vector<Car>& getAllCars() const;

    void loadFromFile();   // cars.json
    void saveToFile();     // cars.json
};

// --- Booking_Manager ---
class Booking_Manager {
    map<int, CarBooking> bookings;
    int nextBookingId;

  public:
    Booking_Manager();

    void addBooking(const CarBooking& booking);
    bool cancelBooking(int bookingId);
    const CarBooking* getBooking(int bookingId) const;
    const map<int, CarBooking>& getAllBookings() const;

    void loadFromFile(CarManager& cm); // bookings.json
    void saveToFile() const;           // bookings.json
};

// --- Admin_c ---
class Admin_c {
    static Admin_c* instance;
    Booking_Manager& bookingManager;
    CarManager& carManager;

    Admin_c(Booking_Manager& bm, CarManager& cm);

  public:
    static Admin_c* getInstance(Booking_Manager& bm,
                                CarManager& cm);

    bool login(const string& uname, const string& pwd);
    void adminMenu();

    // Car
    void viewAllCars() const;
    void addCar();
    void removeCar();

    // Booking
    void viewAllBookings() const;
    void cancelAnyBooking();

    // Route
    void addRoute();

    // helpers
    void loadCars()    { carManager.loadFromFile(); }
    void saveCars()    { carManager.saveToFile(); }
    void loadBookings(){ bookingManager.loadFromFile(carManager); }
    void saveBookings(){ bookingManager.saveToFile(); }

    void loadRoutes()  { TravelRoute::loadRoutesFromFile(); }
    void saveRoutes()  { TravelRoute::saveRoutesToFile(); }

    Car* findCarByReg(const string& reg) {
        return carManager.findCarByReg(reg);
    }
    const vector<Car>& getAllCars() const {
        return carManager.getAllCars();
    }
};

// --- User_c ---
class User_c {
    string username;
    Booking_Manager& bookingManager;
    Admin_c& adminSystem;

    void displayAvailableCars() const;
    void displayAvailableRoutes() const;

  public:
    User_c(const string& name,
           Booking_Manager& bm,
           Admin_c& admin);

    void userMenu();
    void makeBooking();
    void viewBookings() const;
    void cancelBooking();
};

#endif // CARRENTAL_H
