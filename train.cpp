#include "train.h"
                                                                    //Parameterized constructor
Train :: Train(string name, string rt, string sched) : trainName(name), route(rt), schedule(sched) {}

void Train :: getTrainDetails() const {                             //Getting train details
    cout << "Train: " << trainName << ", Route: " << route << ", Schedule: " << schedule << endl;
}

void Train :: updateSchedule(string newSchedule) {                  //Setting schedule
    schedule = newSchedule;
}
                                                                    //Parameterized constructor
LocalTrain :: LocalTrain(string name, string rt, string sched) : Train(name, rt, sched) {}

void LocalTrain :: display() const {                                //Getting local train details
    cout << "Local Train: ";
    getTrainDetails();
}
                                                                    //Parameterized constructor
ExpressTrain :: ExpressTrain(string name, string rt, string sched) : Train(name, rt, sched) {}

void ExpressTrain :: display() const {                              //Getting express train details
    cout << "Express Train: ";
    getTrainDetails();
}
                                                                    //Parameterized constructor
TrainSeat :: TrainSeat(string seatNum) : seatNumber(seatNum), isBooked(false) {}

bool TrainSeat :: isAvailable() const {                             //Getting is available
    return !isBooked;
}

void TrainSeat :: reserveSeat() {                                   //Setting booking
    isBooked = true;
}

void TrainSeat :: cancelSeat() {                                    //Setting booking
    isBooked = false;
}

string TrainSeat :: getSeatNumber() const {                         //Getting seat number
    return seatNumber;
}
                                                                    //Parameterized constructor
RouteInformation :: RouteInformation(string start, string dest, double dist, string time) : startLocation(start), destination(dest), distance(dist), estimatedTime(time) {}

void RouteInformation :: getRouteDetails() const {                  //Getting route
    cout << "Route: " << startLocation << " -> " << destination << " | Distance: " << distance << " km | ETA: " << estimatedTime << endl;
}
                                                                    //Parameterized constructor
TrainBooking :: TrainBooking(string num, double pr, int seatsNum, Train* tr) : trainNumber(num), price(pr), totalSeats(seatsNum), bookedSeats(0), train(tr) {
    for (int i = 0; i < totalSeats; ++i) {
        seats.emplace_back("Seat" + to_string(i + 1));
        seatStatusMap[i + 1] = "Available";
    }
}

bool TrainBooking :: checkAvailability() const {                    //Checking availability
    return bookedSeats<totalSeats;
}

double TrainBooking :: getPrice() const {                           //Getting price
    return price;
}

void TrainBooking :: bookTicket(int num) {                          //Booking a seat
    if (num < 1 || num > totalSeats || !seats[num - 1].isAvailable()) {
        cout << "Seat not available!" << endl;
        return;
    }
    seats[num - 1].reserveSeat();
    bookedSeats++;
    cout << "Seat " << num << " booked successfully!" << endl;
    seatStatusMap[num] = "Booked";
    saveToFile();
}

void TrainBooking :: cancelTicket(int num) {                        //Canceling seat
    if (num < 1 || num > totalSeats || seats[num - 1].isAvailable()) {
        cout << "Seat not booked!" << endl;
        return;
    }
    seats[num - 1].cancelSeat();
    bookedSeats--;
    cout << "Seat " << num << " canceled successfully!" << endl;
    seatStatusMap[num] = "Available";
    saveToFile();
}

void TrainBooking :: viewTrainBooking() const {                     //Getting seat status
    train->getTrainDetails();
    cout << "Seats: " << totalSeats << ", Booked: " << bookedSeats << endl;
    for (const auto& seat : seats) {
        cout << seat.getSeatNumber() << " - " << (seat.isAvailable() ? "Available" : "Booked") << endl;
    }
}

void TrainBooking :: saveToFile() {                                 //Saving data to JSON file
    json j;
    j["trainNumber"] = trainNumber;
    j["price"] = price;
    j["totalSeats"] = totalSeats;
    j["bookedSeats"] = bookedSeats;
    vector<string> seatStatus;
    for (auto& seat : seats) {
        seatStatus.push_back(seat.isAvailable() ? "0" : "1");
    }
    j["seats"] = seatStatus;
    ofstream file("booking.json");
    if (!file.is_open()) {
        cout << "Error: Could not open booking.json for writing!" << endl;
        return;
    }
    file << j.dump(4);
    file.close();
    cout << "Booking saved successfully to booking.json." << endl;
}

TrainBooking TrainBooking :: loadFromFile(string filename) {        //Loading data from JSON file
    ifstream file("booking.json");
    if (!file.is_open()) {
        cout << "Error: Could not open booking.json for reading!" << endl;
    }
    json j;
    file >> j;
    file.close();
    Train* tempTrain = new Train("LoadedTrain", "LoadedRoute", "12:00");
    TrainBooking booking(j["trainNumber"], j["price"], j["totalSeats"], tempTrain);
    booking.bookedSeats = j["bookedSeats"];
    for (int i = 0; i < booking.totalSeats; ++i) {
        if (j["seats"][i] == "1") {
            booking.seats[i].reserveSeat();
            booking.seatStatusMap[i + 1] = "Booked";
        }
        else {
            booking.seatStatusMap[i + 1] = "Available";
        }
    }
    cout << "Booking loaded successfully from booking.json." << endl;
    return booking;
}

void TrainBooking :: viewSeatStatus() const {                       //Getting status of seats
    cout << "Seat Status (from map):" << endl;
    for (const auto& seat : seatStatusMap) {
        cout << "Seat " << seat.first << " → " << seat.second << endl;
    }
}

void TrainBooking :: addPassenger(const Passenger& p) {             //Adding passenger
    passengers.push_back(p);
    cout << "Passenger added successfully." << endl;
}

void TrainBooking :: viewPassengers() const {                       //Viewing passengers
    if (passengers.empty()) {
        cout << "No passengers associated with this booking." << endl;
        return;
    }
    cout << "Passengers:" << endl;
    for (const auto& p : passengers) {
        p.displayInfo();
    }
}

SystemInfo* SystemInfo :: instance = nullptr;                       //Initializing pointer to null

SystemInfo :: SystemInfo() {                                        //System info
    version = "1.0";
    adminEmail = "admin@trainbookingsystem.com";
    totalTrains = 1;
}

SystemInfo* SystemInfo :: getInstance() {                           //Singleton class
    if (instance == nullptr) {
        instance = new SystemInfo();
    }
    return instance;
}

void SystemInfo :: showInfo() const {                               //Displaying system info
    cout << "Train Booking System - Version: " << version << endl;
    cout << "Contact Admin: " << adminEmail << endl;
    cout << "Total Trains Available: " << totalTrains << endl;
}

void SystemInfo :: setTotalTrains(int t) {                          //Setting total trains
    totalTrains = t;
}

void trainSystemMenu(bool isAdmin) {
    LocalTrain local("Local 101", "Lahore - Karachi", "10:00 AM");
    ExpressTrain express("Express 202", "Lahore - Islamabad", "02:00 PM");

    cout << "Choose train type:" << endl;
    cout << "1. Local Train" << endl;
    cout << "2. Express Train" << endl;
    cout << "Choice: ";
    int trainType;
    cin >> trainType;

    Train* selectedTrain = nullptr;
    if (trainType == 1) {
        selectedTrain = &local;
    }
    else if (trainType == 2) {
        selectedTrain = &express;
    }
    else {
        cout << "Invalid choice. Defaulting to Local Train." << endl;
        selectedTrain = &local;
    }

    TrainBooking booking("T001", 250.0, 5, selectedTrain);

    bool exitProgram = false;
    while (!exitProgram) {
        if (isAdmin) {
            cout << "--- Admin Menu ---" << endl;
            cout << "1. View Train Details" << endl;
            cout << "2. Update Train Schedule" << endl;
            cout << "3. Change Ticket Fare" << endl;
            cout << "4. View All Bookings" << endl;
            cout << "5. Save Booking to File" << endl;
            cout << "6. Show System Info" << endl;
            cout << "7. Change Number of Seats" << endl;
            cout << "8. Add New Train" << endl;
            cout << "9. Logout" << endl;
            cout << "0. Exit Program" << endl;
        }
        else {
            cout << "--- User Menu ---" << endl;
            cout << "1. View Train Details" << endl;
            cout << "2. Book a Seat" << endl;
            cout << "3. Cancel a Seat" << endl;
            cout << "4. View Seat Status" << endl;
            cout << "5. Load Booking from File" << endl;
            cout << "6. Add Passenger" << endl;
            cout << "7. View Passengers" << endl;
            cout << "8. Logout" << endl;
            cout << "0. Exit Program" << endl;
        }

        cout << "Enter your choice: ";
        int choice;
        cin >> choice;

        if (isAdmin) {
            switch (choice) {
                case 1:
                    booking.viewTrainBooking();
                    break;
                case 2: {
                    cin.ignore();
                    string newSchedule;
                    cout << "Enter new schedule: ";
                    getline(cin, newSchedule);
                    selectedTrain->updateSchedule(newSchedule);
                    cout << "Schedule updated." << endl;
                    break;
                }
                case 3: {
                    double newFare;
                    cout << "Enter new fare: ";
                    cin >> newFare;
                    booking = TrainBooking("T001", newFare, booking.totalSeats, selectedTrain);
                    cout << "Fare updated." << endl;
                    break;
                }
                case 4:
                    booking.viewTrainBooking();
                    break;
                case 5:
                    booking.saveToFile();
                    break;
                case 6:
                    SystemInfo::getInstance()->showInfo();
                    break;
                case 7: {
                    int newSeats;
                    cout << "Enter new number of seats: ";
                    cin >> newSeats;
                    booking = TrainBooking("T001", booking.getPrice(), newSeats, selectedTrain);
                    cout << "Seat count updated." << endl;
                    break;
                }
                case 8: {
                    int newTrainType;
                    string name, route, schedule;
                    cin.ignore();
                    cout << "Enter train type (1: Local, 2: Express): ";
                    cin >> newTrainType;
                    cin.ignore();
                    cout << "Enter train name: ";
                    getline(cin, name);
                    cout << "Enter route: ";
                    getline(cin, route);
                    cout << "Enter schedule: ";
                    getline(cin, schedule);
                    if (newTrainType == 1) {
                        LocalTrain local(name, route, schedule);
                        *selectedTrain = local;
                    }
                    else if (newTrainType == 2) {
                        ExpressTrain express(name, route, schedule);
                        *selectedTrain = express;
                    }
                    else cout << "Invalid train type entered." << endl;
                    booking = TrainBooking("T001", booking.getPrice(), booking.totalSeats, selectedTrain);
                    cout << "New train added successfully." << endl;
                    break;
                }
                case 9:
                    cout << "Logging out..." << endl;
                    exitProgram = true;
                    break;
                case 0:
                    cout << "Exiting program..." << endl;
                    exit(0);
                default:
                    cout << "Invalid option!" << endl;
            }
        }
        else {
            switch (choice) {
                case 1:
                    booking.viewTrainBooking();
                    break;
                case 2: {
                    int seat;
                    cout << "Enter seat number to book: ";
                    cin >> seat;
                    booking.bookTicket(seat);
                    booking.saveToFile();
                    break;
                }
                case 3: {
                    int seat;
                    cout << "Enter seat number to cancel: ";
                    cin >> seat;
                    booking.cancelTicket(seat);
                    booking.saveToFile();
                    break;
                }
                case 4:
                    booking.viewSeatStatus();
                    break;
                case 5:
                    booking = TrainBooking::loadFromFile("booking.json");
                    break;
                case 6: {
                    cin.ignore();
                    string name;
                    int age;
                    cout << "Enter passenger name: ";
                    getline(cin, name);
                    cout << "Enter passenger age: ";
                    cin >> age;
                    Passenger p(name, age);
                    booking.addPassenger(p);
                    break;
                }
                case 7:
                    booking.viewPassengers();
                    break;
                case 8:
                    cout << "Logging out..." << endl;
                    exitProgram = true;
                    break;
                case 0:
                    cout << "Exiting program..." << endl;
                    exit(0);
                default:
                    cout << "Invalid option!" << endl;
            }
        }
    }
}