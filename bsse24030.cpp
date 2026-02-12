#include "bsse24030.h"
#include <fstream>
#include <algorithm>

using namespace std;

// ─── TravelRoute ─────────────────────────────────────────────────────────────

map<string, TravelRoute> TravelRoute::routeData;

TravelRoute::TravelRoute()
 : from(""), to(""), routePlan(""), distanceKm(0), baseRatePerKm(0)
{}

TravelRoute::TravelRoute(const string& f,
                         const string& t,
                         double dist,
                         double rate)
 : from(f), to(t)
 , distanceKm(dist), baseRatePerKm(rate)
{
    generateRoute();
}

void TravelRoute::generateRoute() {
    routePlan = "Route from " + from + " to " + to +
                " [" + to_string(distanceKm) + " km]";
}

void TravelRoute::displayRoute() const {
    cout << routePlan
         << " Base level Price: " << getEstimatedPrice() << "\n";
}

double TravelRoute::getEstimatedPrice() const {
    return distanceKm * baseRatePerKm;
}

string TravelRoute::getFrom() const { return from; }
string TravelRoute::getTo()   const { return to;   }
string TravelRoute::getRoutePlan() const { return routePlan; }

string TravelRoute::getKey() const {
    return from + "->" + to;
}

// JSON persistence
void TravelRoute::loadRoutesFromFile() {
    ifstream file("routes.json");
    if (!file) return;
    json j; file >> j;

    routeData.clear();
    for (json::iterator it = j.begin(); it != j.end(); ++it) {
        string key = it.key();
        auto &v = it.value();
        routeData[key] = TravelRoute(
            v["from"].get<string>(),
            v["to"].get<string>(),
            v["distance"].get<double>(),
            v["rate"].get<double>()
        );
    }
}

void TravelRoute::saveRoutesToFile() {
    json j;
    for (map<string,TravelRoute>::const_iterator it = routeData.begin();
         it != routeData.end(); ++it)
    {
        const TravelRoute &r = it->second;
        j[it->first] = {
          {"from",     r.getFrom()},
          {"to",       r.getTo()},
          {"distance", r.distanceKm},
          {"rate",     r.baseRatePerKm}
        };
    }
    ofstream file("routes.json");
    file << j.dump(4);
}

TravelRoute TravelRoute::findRoute(const string& f, const string& t) {
    string key = f + "->" + t;
    auto it = routeData.find(key);
    return (it != routeData.end())
         ? it->second
         : TravelRoute(f,t,0,0);
}

// ─── Car ─────────────────────────────────────────────────────────────────────

Car::Car(string m, string reg, double mult, bool avail)
 : model(m), registration(reg)
 , priceMultiplier(mult), available(avail)
{}

string Car::getModel() const         { return model; }
string Car::getRegistration() const  { return registration; }
bool   Car::isAvailable() const      { return available; }
double Car::getPriceMultiplier() const { return priceMultiplier; }

void Car::markAvailable()   { available = true; }
void Car::markUnavailable() { available = false; }

// ─── CarBooking ─────────────────────────────────────────────────────────────

int CarBooking::nextId = 1;


CarBooking::CarBooking(const string& u, const Car& c, const TravelRoute& r)
 : username(u), car(c), route(r), totalPrice(0), bookingId(nextId++)
{}

int    CarBooking::getId()           const { return bookingId; }
const string& CarBooking::getUsername() const { return username; }
const TravelRoute& CarBooking::getRoute()    const { return route; }
const Car&       CarBooking::getCar()        const { return car; }
double           CarBooking::getTotalPrice() const { return totalPrice; }

void CarBooking::calculatePrice() {
    totalPrice = route.getEstimatedPrice()
               * car.getPriceMultiplier();
}

// ─── CarManager ──────────────────────────────────────────────────────────────

void CarManager::addCar(const Car& car) {
    cars.push_back(car);
}

void CarManager::removeCar(const string& reg) {
    cars.erase(
      remove_if(cars.begin(), cars.end(),
        [&](const Car& c){ return c.getRegistration()==reg; }),
      cars.end()
    );
}

Car* CarManager::findCarByReg(const string& reg) {
    for (size_t i=0; i<cars.size(); ++i)
        if (cars[i].getRegistration()==reg)
            return &cars[i];
    return NULL;
}

const vector<Car>& CarManager::getAllCars() const {
    return cars;
}

void CarManager::loadFromFile() {
    ifstream file("cars.json");
    if (!file) return;
    json j; file >> j;
    cars.clear();
    for (json::iterator it=j.begin(); it!=j.end(); ++it) {
        auto &v = it.value();
        cars.emplace_back(
            v["model"].get<string>(),
            it.key(),
            v["price_multiplier"].get<double>(),
            v["available"].get<bool>()
        );
    }
}

void CarManager::saveToFile() {
    json j;
    for (size_t i=0; i<cars.size(); ++i) {
        const Car &c = cars[i];
        j[c.getRegistration()] = {
            {"model", c.getModel()},
            {"price_multiplier", c.getPriceMultiplier()},
            {"available", c.isAvailable()}
        };
    }
    ofstream file("cars.json");
    file << j.dump(4);
}

// ─── Booking_Manager ─────────────────────────────────────────────────────────

Booking_Manager::Booking_Manager()
 : nextBookingId(1)
{}

void Booking_Manager::addBooking(const CarBooking& b) {
    bookings[b.getId()] = b;
    if (b.getId()>=nextBookingId) nextBookingId=b.getId()+1;
}

bool Booking_Manager::cancelBooking(int id) {
    return bookings.erase(id)>0;
}

const CarBooking* Booking_Manager::getBooking(int id) const {
    auto it = bookings.find(id);
    return (it!=bookings.end()) ? &it->second : NULL;
}

const map<int,CarBooking>& Booking_Manager::getAllBookings() const {
    return bookings;
}

void Booking_Manager::loadFromFile(CarManager& cm) {
    ifstream file("bookings.json");
    if (!file) return;
    json j; file>>j;
    bookings.clear();
    nextBookingId=1;
    for (json::iterator it=j.begin(); it!=j.end(); ++it) {
        int id = stoi(it.key());
        auto &v = it.value();
        TravelRoute r = TravelRoute::findRoute(
            v["departure"].get<string>(),
            v["destination"].get<string>()
        );
        Car* c = cm.findCarByReg(v["car_reg"].get<string>());
        if(!c) continue;
        CarBooking b(v["user"].get<string>(), *c, r);
        b.calculatePrice();
        bookings[id]=b;
        if (id>=nextBookingId) nextBookingId=id+1;
    }
}

void Booking_Manager::saveToFile() const {
    json j;
    for (map<int,CarBooking>::const_iterator it=bookings.begin();
         it!=bookings.end(); ++it)
    {
        const CarBooking &b = it->second;
        j[to_string(it->first)] = {
          {"user",        b.getUsername()},
          {"departure",   b.getRoute().getFrom()},
          {"destination", b.getRoute().getTo()},
          {"car_reg",     b.getCar().getRegistration()},
          {"total_price", b.getTotalPrice()}
        };
    }
    ofstream file("bookings.json");
    file<<j.dump(4);
}

// ─── Admin_c ─────────────────────────────────────────────────────────────────

Admin_c* Admin_c::instance = NULL;

Admin_c::Admin_c(Booking_Manager& bm, CarManager& cm)
 : bookingManager(bm), carManager(cm)
{}

Admin_c* Admin_c::getInstance(Booking_Manager& bm, CarManager& cm) {
    if(!instance) instance = new Admin_c(bm,cm);
    return instance;
}

bool Admin_c::login(const string& u, const string& p) {
    return u=="admin" && p=="admin123";
}

void Admin_c::adminMenu() {
    loadCars();  loadBookings();  loadRoutes();
    int choice;
    do {
        cout<<"\n--- Admin Menu ---\n"
            "1) View Cars  2) Add Car  3) Remove Car\n"
            "4) View Bookings 5) Cancel Booking\n"
            "6) Add Route   0) Logout\n"
            "Choice> ";
        cin>>choice;
        switch(choice) {
         case 1: viewAllCars(); break;
         case 2: addCar();     break;
         case 3: removeCar();  break;
         case 4: viewAllBookings(); break;
         case 5: cancelAnyBooking();break;
         case 6: addRoute();  break;
        }
    } while(choice!=0);
    saveCars(); saveBookings(); saveRoutes();
}

void Admin_c::viewAllCars() const {
    cout<<"\nCars:\n";
    for(size_t i=0;i<carManager.getAllCars().size();++i) {
        const Car &c=carManager.getAllCars()[i];
        cout<<c.getModel()<<"("<<c.getRegistration()<<")"
            <<" x"<<c.getPriceMultiplier()
            <<" ["<<(c.isAvailable()?"Avail":"Busy")<<"]\n";
    }
}

void Admin_c::addCar() {
    string m, reg; double mult; bool av;
    cout<<"Model: ";cin>>m;
    cout<<"Reg: ";  cin>>reg;
    cout<<"Multiplier: ";cin>>mult;
    cout<<"Available? (1/0): ";cin>>av;
    carManager.addCar(Car(m,reg,mult,av));
}

void Admin_c::removeCar() {
    string reg; cout<<"Reg to remove: ";cin>>reg;
    carManager.removeCar(reg);
}

void Admin_c::viewAllBookings() const {
    cout<<"\nBookings:\n";
    for(auto it=bookingManager.getAllBookings().begin();
        it!=bookingManager.getAllBookings().end(); ++it)
    {
        const CarBooking &b=it->second;
        cout<<"ID:"<<it->first
            <<" User:"<<b.getUsername()
            <<" "<<b.getRoute().getKey()
            <<" Car:"<<b.getCar().getRegistration()
            <<" Price:"<<b.getTotalPrice()<<"\n";
    }
}

void Admin_c::cancelAnyBooking() {
    int id; cout<<"Booking ID to cancel: ";cin>>id;
    bookingManager.cancelBooking(id);
}

void Admin_c::addRoute() {
    string from, to;
    double dist, rate;
    cout << "From: ";      cin >> from;
    cout << "To: ";        cin >> to;
    cout << "Distance: ";  cin >> dist;
    cout << "Rate per km: "; cin >> rate;

    TravelRoute r(from, to, dist, rate);
    // <— fully qualify the static map:
    TravelRoute::routeData[r.getKey()] = r;
    cout << "Route added.\n";
}

// ─── User_c ─────────────────────────────────────────────────────────────────

User_c::User_c(const string& n,
               Booking_Manager& bm,
               Admin_c& admin)
 : username(n), bookingManager(bm), adminSystem(admin)
{}

void User_c::userMenu() {
    int choice;
    do {
        cout<<"\n--- User Menu ---\n"
            "1) View Cars  2) View Routes\n"
            "3) Book Car   4) My Bookings\n"
            "5) Cancel Booking 0) Logout\n"
            "Choice> ";
        cin>>choice;
        switch(choice) {
          case 1: displayAvailableCars(); break;
          case 2: displayAvailableRoutes();break;
          case 3: makeBooking();break;
          case 4: viewBookings(); break;
          case 5: cancelBooking();break;
          case 0: bookingManager.saveToFile(); break;
        }
    } while(choice!=0);
}

void User_c::displayAvailableCars() const {
    cout<<"\nAvailable Cars:\n";
    for(size_t i=0;i<adminSystem.getAllCars().size();++i){
        const Car &c=adminSystem.getAllCars()[i];
        if(c.isAvailable())
            cout<<c.getModel()<<"("<<c.getRegistration()<<") x"
                <<c.getPriceMultiplier()<<"\n";
    }
}

void User_c::displayAvailableRoutes() const {
    cout<<"\nRoutes:\n";
    for(auto it=TravelRoute::routeData.begin();
        it!=TravelRoute::routeData.end(); ++it)
    {
        const TravelRoute &r=it->second;
        cout<<r.getKey()<<" Price: "<<r.getEstimatedPrice()<<"\n";
    }
}

void User_c::makeBooking() {
    string f,t,reg;
    cout<<"From: ";cin>>f;
    cout<<"To: ";  cin>>t;
    cout<<"Car Reg: ";cin>>reg;
    TravelRoute r=TravelRoute::findRoute(f,t);
    Car* cptr=adminSystem.findCarByReg(reg);
    if(!cptr || !cptr->isAvailable()){
        cout<<"Invalid car or route.\n"; return;
    }
    CarBooking b(username,*cptr,r);
    b.calculatePrice();
    bookingManager.addBooking(b);
    bookingManager.saveToFile();

    cptr->markUnavailable();
    cout<<"Booked ID="<<b.getId()
        <<" Price="<<b.getTotalPrice()<<"\n";

}

void User_c::viewBookings() const {
    cout<<"\nMy Bookings:\n";
    for(auto it=bookingManager.getAllBookings().begin();
        it!=bookingManager.getAllBookings().end(); ++it)
    {
        const CarBooking &b=it->second;
        if(b.getUsername()==username) {
            cout<<"ID:"<<it->first
                <<" Route:"<<b.getRoute().getKey()
                <<" Car:"<<b.getCar().getRegistration()
                <<" New Total Price:"<<b.getTotalPrice()<<"\n";
        }
    }
}

void User_c::cancelBooking(){
    int id; cout<<"Booking ID: ";cin>>id;
    if(bookingManager.cancelBooking(id))
        cout<<"Cancelled.\n";
    else
        cout<<"Not found.\n";
}
