#include "bsse24046.h"
// --- User Methods ---
User_r::User_r(string uname, string pass) : username(uname), password(pass) {}

bool User_r::checkPassword(const string& pass) const {
    return password == pass;
}

void User_r::addBooking(const HotelBooking& b) {
    bookings.push_back(b);
}

const vector<HotelBooking>& User_r::getBookings() const {
    return bookings;
}

string User_r::getUsername() const {
    return username;
}

void to_json(json& j, const User_r& u) {
    j = json{{"username", u.username}, {"password", u.password}, {"bookings", u.bookings}};
}

void from_json(const json& j, User_r& u) {
    j.at("username").get_to(u.username);
    j.at("password").get_to(u.password);
    j.at("bookings").get_to(u.bookings);
}
// --- Static Singleton Instance ---
HotelManager* HotelManager::instance = nullptr;

// --- Room Methods ---
Room::Room(string num, string t, double p)
    : roomNumber(num), isBooked(false), type(t), price(p) {}
bool Room::isAvailable() const { return !isBooked; }

void Room::reserve() { isBooked = true; }

void Room::cancel() { isBooked = false; }

string Room::getRoomNumber() const { return roomNumber; }

// --- HotelBooking Methods ---
HotelBooking::HotelBooking(double pr, int n)
    :price(pr), nights(n), room(nullptr) {}

void HotelBooking::assignRoom(Room* r) {
    room = r;
    if (room) room->reserve();
}

bool HotelBooking::cancelBooking() {
    if (room) {
        room->cancel();
        return true;
    }
    return false;
}

void HotelBooking::display() const {
    cout << "Room Type: " << (room ? room->getType() : "N/A")
         << " | Nights: " << nights
         << " | Rate: PKR " << price
         << " | Total: PKR " << price * nights;
    if (room) cout << " | Room #: " << room->getRoomNumber();
    cout << endl;
}

void to_json(json& j, const HotelBooking& b) {
    j = json{
        {"price", b.price},
        {"nights", b.nights},
        {"roomNumber", b.room ? b.room->getRoomNumber() : ""}
    };
}

void from_json(const json& j, HotelBooking& b) {
    j.at("price").get_to(b.price);
    j.at("nights").get_to(b.nights);

    string roomNum;
    j.at("roomNumber").get_to(roomNum);

    HotelManager* manager = HotelManager::getInstance();
    for (Hotel* hotel : manager->getHotels()) {
        for (Room& room : hotel->getRooms()) {
            if (room.getRoomNumber() == roomNum) {
                b.room = &room;
                room.reserve();  // Mark as booked
                return;
            }
        }
    }

    // if not found
    b.room = nullptr;
}


// --- Hotel Methods ---
Hotel::Hotel(string name, string loc, int r)
    : hotelName(name), location(loc), rating(r) {}

void Hotel::display() const {
    cout << hotelName << " (" << location << ") Rating: " << rating << "/5" << endl;
    for (const Room& r : rooms) {
        cout << "  Room#: " << r.getRoomNumber()
             << " | Type: " << r.getType()
             << " | Price per night: PKR " << r.getPrice()
             << " | Status: " << (r.getIsBooked() ? "Booked" : "Available") << "\n";
    }
}

void Hotel::addRoom(Room r) {
    rooms.push_back(r);
}

Room* Hotel::findAvailableRoom() {
    for (auto& room : rooms)
        if (room.isAvailable()) return &room;
    return nullptr;
}

string Hotel::getName() const {
    return hotelName;
}
string Hotel::getLocation() const {
    return location;
}
int Hotel::getRating() const {
    return rating;
}
void Hotel::editDetails() {
    cout << "Current Hotel Name: " << hotelName << endl;
    cout << "Enter new hotel name: ";
    getline(cin, hotelName);

    cout << "Current Location: " << location << endl;
    cout << "Enter new location: ";
    getline(cin, location);

    cout << "Current Rating: " << rating << endl;
    cout << "Enter new rating (1-5): ";
    cin >> rating;
}
vector<Room>& Hotel::getRooms() {
    return rooms;
}

// --- Derived Classes ---
LuxuryHotel::LuxuryHotel(string name, string loc) : Hotel(name, loc, 5) {}

void LuxuryHotel::display() const {
    cout << "[Luxury] ";
    Hotel::display();
}

StandardHotel::StandardHotel(string name, string loc) : Hotel(name, loc, 3) {}

void StandardHotel::display() const {
    cout << "[Standard] ";
    Hotel::display();
}

// --- Singleton ---
HotelManager* HotelManager::getInstance() {
    if (!instance) instance = new HotelManager();
    return instance;
}

void HotelManager::addHotel(Hotel* h) {
    hotels.push_back(h);
}

vector<Hotel*>& HotelManager::getHotels() {
    return hotels;
}
//Map
map<string, User_r> userDatabase;
//Exception
void loadUsers() {
    try {
        ifstream in("users.json");
        if (!in.is_open()) throw runtime_error("Failed to open users.json");

        json j;
        in >> j;
        for (const auto& item : j) {
            User_r u = item;
            userDatabase[u.getUsername()] = u;
        }
    } catch (const exception& e) {
        cerr << "Error loading users: " << e.what() << endl;
    }
}

void saveUsers() {
    try {
        ofstream out("users.json");
        if (!out.is_open()) throw runtime_error("Failed to open users.json for writing");

        json j;

        for (const auto& pair : userDatabase) {
            const User_r& user = pair.second;
            j.push_back(user); 
        }
        out << j.dump(4);  
        out.close();
    } catch (const exception& e) {
        cerr << "Error saving users: " << e.what() << endl;
    }
}
void saveHotels() {
    json j;
    HotelManager* manager = HotelManager::getInstance();

    for (Hotel* hotel : manager->getHotels()) {
        json hotelJson;
        hotelJson["hotelName"] = hotel->getName();
        hotelJson["location"] = hotel->getLocation();
        hotelJson["rating"] = hotel->getRating();

        json roomsJson = json::array();
        for (Room& room : hotel->getRooms()) {
            roomsJson.push_back({
                {"roomNumber", room.getRoomNumber()},
                {"isBooked", !room.isAvailable()},
                {"type", room.getType()},
                {"price", room.getPrice()}
            });
        }

        hotelJson["rooms"] = roomsJson;
        j.push_back(hotelJson);
    }

    ofstream out("hotels.json");
    out << j.dump(4);
}

void loadHotels() {
    ifstream in("hotels.json");
    if (!in) return;

    json j;
    in >> j;

    HotelManager* manager = HotelManager::getInstance();

    for (const auto& hotelJson : j) {
        string hotelName = hotelJson["hotelName"];
        string location = hotelJson["location"];
        int rating = hotelJson["rating"];

        Hotel* hotel = nullptr;
        if (rating == 5) hotel = new LuxuryHotel(hotelName, location);
        else hotel = new StandardHotel(hotelName, location);
        if (!hotel) continue;

        for (const auto& roomJson : hotelJson["rooms"]) {
            string roomNumber = roomJson["roomNumber"];
            bool isBooked = roomJson["isBooked"];
            string type = roomJson.value("type", "Standard");
            double price = roomJson.value("price", 100.0);

            Room room(roomNumber, type, price);
            if (isBooked) room.reserve();
            hotel->addRoom(room);
        }

        manager->addHotel(hotel);
    }
}
TotalAmount<double> totalAmount;
void loadTotalAmount() {
    ifstream in("revenue.json");
    if (!in) return;
    
    double value;
    in >> value;
    totalAmount = TotalAmount<double>(value);
}

void saveTotalAmount() {
    ofstream out("revenue.json");
    if (!out) return;
    out << totalAmount.getTotal();  
}


// --- Menu Function ---
void hotelBookingMenu(bool isAdmin) {
    static bool initialized = false;
    if (!initialized) {
        //Firstly Loading the previous data
        loadHotels();
        loadUsers();
        loadTotalAmount();
        initialized = true;
    }

    User_r* currentUser = nullptr;
    if (!isAdmin) {
        string uname, pass;
        cout << "Username: ";
        cin.ignore();
        getline(cin, uname);
        cout << "Password: ";
        getline(cin, pass);

        if (userDatabase.count(uname) && userDatabase[uname].checkPassword(pass)) {
            currentUser = &userDatabase[uname];
            cout << "Welcome back, " << uname << "!\n";
        } else {
            cout << "New user created.\n";
            userDatabase[uname] = User_r(uname, pass);
            currentUser = &userDatabase[uname];
        }
    }

    int choice;
    do {
        cout << "\n--- Hotel Management Menu ---\n";
        cout << "1. View Hotels & Details\n";

        if (!isAdmin) {
            cout << "2. Book a Room\n";
            cout << "3. View My Bookings\n";
            cout << "4. Cancel a Booking\n";
        } else {
            cout << "2. Add Hotel\n";
            cout << "3. Remove Hotel\n";
            cout << "4. Add Room to Hotel\n";
            cout << "5. Edit Hotel Details\n";
            cout << "6. See the total amount of money made.\n";
        }

        cout << "0. Back to Main Menu\n";
        cout << "Enter choice: ";
        cin >> choice;

        HotelManager* manager = HotelManager::getInstance();

        if (choice == 1) {
            for (Hotel* h : manager->getHotels()) {
                h->display();
                int total = h->getRooms().size();
                int booked = 0;
                for (Room& r : h->getRooms()) {
                    if (!r.isAvailable()) booked++;
                }
                cout << "Total Rooms: " << total
                     << ", Booked: " << booked
                     << ", Available: " << (total - booked) << endl;
                cout << "-----------------------------\n";
            }

        } else if (choice == 2) {
            if (!isAdmin) {
                vector<Hotel*>& hotels = manager->getHotels();
                if (hotels.empty()) {
                    cout << "No hotels available.\n";
                    continue;
                }
        
                cout << "Available Hotels:\n";
                for (size_t i = 0; i < hotels.size(); ++i) {
                    cout << i + 1 << ". " << hotels[i]->getName()
                         << " (" << hotels[i]->getLocation() << "), Rating: "
                         << hotels[i]->getRating() << "\n";
                }
        
                int hChoice;
                cout << "Select a hotel by number: ";
                cin >> hChoice;
                cin.ignore();
                if (hChoice < 1 || hChoice > hotels.size()) {
                    cout << "Invalid hotel selection.\n";
                    continue;
                }
        
                Hotel* selected = hotels[hChoice - 1];
                vector<Room*> availableRooms;
                cout << "Available Rooms:\n";
                for (Room& r : selected->getRooms()) {
                    if (r.isAvailable()) {
                        int index = availableRooms.size() + 1;
                        double price=r;
                        cout << index << ". Room#: " << r.getRoomNumber()
                             << " | Type: " << r.getType()
                             << " | Price per night: PKR " << price << "\n";
                        availableRooms.push_back(&r);
                    }
                }
        
                if (availableRooms.empty()) {
                    cout << "No available rooms in this hotel.\n";
                    continue;
                }
        
                int rChoice;
                cout << "Select a room by number: ";
                cin >> rChoice;
                cin.ignore();
                if (rChoice < 1 || rChoice > availableRooms.size()) {
                    cout << "Invalid room selection.\n";
                    continue;
                }
        
                int nights;
                cout << "Enter number of nights: ";
                cin >> nights;
                cin.ignore();
                if (nights <= 0) {
                    cout << "Invalid number of nights.\n";
                    continue;
                }
        
                Room* selectedRoom = availableRooms[rChoice - 1];
                HotelBooking booking(selectedRoom->getPrice(), nights);
                booking.assignRoom(selectedRoom);
                currentUser->addBooking(booking);
                totalAmount = totalAmount + booking.getPrice();
                saveTotalAmount();
                saveUsers();
        
                cout << "Room booked in " << selected->getName() << "!\n";
                
            }
         else {
                string name, loc;
                int rating;
                cout << "Enter Hotel Name: ";
                cin.ignore();
                getline(cin, name);
                cout << "Enter Location: ";
                getline(cin, loc);
                cout << "Enter Rating (1-5): ";
                cin >> rating;

                Hotel* newHotel = nullptr;
                if (rating >= 5) newHotel = new LuxuryHotel(name, loc);
                else newHotel = new StandardHotel(name, loc);

                manager->addHotel(newHotel);
                saveHotels();
                cout << "Hotel added.\n";
            }

        } else if (choice == 3) {
            if (!isAdmin) {
                const auto& bookings = currentUser->getBookings();
                if (bookings.empty()) {
                    cout << "No bookings yet.\n";
                } else {
                    for (const auto& b : bookings) b.display();
                }
            } else {
                string name;
                cout << "Enter hotel name to remove: ";
                cin.ignore();
                getline(cin, name);

                auto& hotels = manager->getHotels();
                auto it = remove_if(hotels.begin(), hotels.end(), [&](Hotel* h) {
                    return h->getName() == name;
                });

                if (it != hotels.end()) {
                    hotels.erase(it, hotels.end());
                    saveHotels();
                    cout << "Hotel removed.\n";
                } else {
                    cout << "Hotel not found.\n";
                }
            }

        } else if (choice == 4 && !isAdmin) {
            auto& bookings = currentUser->getBookings();
            if (bookings.empty()) {
                cout << "No bookings to cancel.\n";
                continue;
            }

            cout << "Your Bookings:\n";
            for (size_t i = 0; i < bookings.size(); ++i) {
                cout << i + 1 << ". ";
                bookings[i].display();
            }

            int cancelIndex;
            cout << "Enter booking number to cancel (0 to skip): ";
            cin >> cancelIndex;

            if (cancelIndex == 0) continue;

            if (cancelIndex < 1 || cancelIndex > bookings.size()) {
                cout << "Invalid booking number.\n";
                continue;
            }

            auto& mutableBookings = const_cast<vector<HotelBooking>&>(bookings);
            if (mutableBookings[cancelIndex - 1].cancelBooking()) {
                double bookingPrice = mutableBookings[cancelIndex - 1].getPrice();
                mutableBookings.erase(mutableBookings.begin() + (cancelIndex - 1));
                totalAmount = totalAmount - bookingPrice;
                saveTotalAmount();
                saveUsers();
                cout << "Booking canceled.\n";
            } else {
                cout << "Cancellation failed.\n";
            }

        } else if (choice == 4 && isAdmin) {
            string name;
            cout << "Enter hotel name to add room to: ";
            cin.ignore();
            getline(cin, name);

            bool found = false;
            for (auto h : manager->getHotels()) {
                if (h->getName() == name) {
                    string roomNum, type;
                    double price;
                    cout << "Enter new room number: ";
                    getline(cin, roomNum);
                    cout << "Enter room type (Single/Double/Suite): ";
                    getline(cin, type);
                    cout << "Enter price per night: ";
                    cin >> price;
                    *h += Room(roomNum, type, price);
                    saveHotels();
                    cout << "Room added.\n";
                    found = true;
                    break;
                }
            }
            if (!found) cout << "Hotel not found.\n";

        } else if (choice == 5 && isAdmin) {
            string name;
            cout << "Enter hotel name to edit: ";
            cin.ignore();
            getline(cin, name);

            bool found = false;
            for (auto h : manager->getHotels()) {
                if (h->getName() == name) {
                    h->editDetails();
                    saveHotels();
                    cout << "Hotel updated.\n";
                    found = true;
                    break;
                }
            }
            if (!found) cout << "Hotel not found.\n";
        } else if (choice == 6 && isAdmin){
            totalAmount.display();
        }
    } while (choice != 0);
    //Saving the data to files
    saveUsers();
    saveHotels();
    saveTotalAmount();
}
