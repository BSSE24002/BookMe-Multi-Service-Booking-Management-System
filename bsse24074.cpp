#include "bsse24074.h"
//========================================== USER CLASS FUNCTIONS======================================
void User::accessFlights(FlightBooking& fb) {
    fb.viewFlights();  // Display available flights

    string flightID;
    int seats, option;
    string method;

    cout << "Enter Flight ID to book: ";
   try {
        cin >> flightID;
        Flight* flight = fb.findFlight(flightID);
        if (!flight) {
            throw invalid_argument(" Flight not found ");
        }
    } catch (const invalid_argument& e) {
        cout << e.what() << endl;
        return;
    }
    cout << "Enter number of seats to book: ";
    try {
        cin >> seats;
        if (seats <= 0) {
            throw invalid_argument("Number of seats must be positive.");
        }
    } catch (const invalid_argument& e) {
        cout << "Error: " << e.what() << "\nBooking cancelled.\n";
        return;  // Exit the function if invalid input
    }
    cout << "You want window seats? (Cost Extra 10%): \n1.Yes \n2.No: ";
    cin >> option;

    if (!flightID.empty()) {  // If flightID is provided, proceed with booking
        Flight* flight = fb.findFlight(flightID);
        double totalAmount = 0.0;  // Initialize totalAmount

        if (flight) {
            // Calculate the base price for the seats
            totalAmount = flight->getPrice() * seats;

            // If the user wants window seats, add 10% to the total amount
            if (option == 1) {
                totalAmount *= 1.10;  // Increase totalAmount by 10%
            }

            cout<<fixed<<setprecision(2) << "Total Amount: $" << totalAmount << "\n" << endl;

            if (makePayment(totalAmount,method)) {
                // After successful payment, proceed with booking
                if (flight->bookSeat(seats)) {
                    flight->setLastBookedSeats(seats);
                    cout << "Flight booked successfully!\n" << "\n";
                    fb.recordPayment(flight, totalAmount,method);
                    fb.saveToFile("flights.json");
                    cout << *flight; // Print Ticket
                } else {
                    cout << "Booking failed. Not enough seats available.\n";
                }
            } else {
                cout << "Payment Issues. Booking Cancelled";
            }
        } else {
            cout << "Invalid Flight ID.\n";
        }
    }
}
//------------------------------------------------------------------------------------------------------------------

bool User::makePayment(double amount,string& method) {
    cout << "Choose Payment Method:\n";
    cout << "1. Credit Card\n2. Debit Card\n3. PayPal\nEnter choice: ";

    int paymentChoice;
    cin >> paymentChoice;

    string paymentMethod;
    switch (paymentChoice) {
        case 1: paymentMethod = "Credit Card"; break;
        case 2: paymentMethod = "Debit Card"; break;
        case 3: paymentMethod = "PayPal"; break;
        default:
            cout << "Invalid option. Payment failed.\n";
        return false;  // Indicate payment failure
    }

    int attempts =3;
    while (attempts--) {
        string cardNumber, expiry, cvv;
        cout << "Enter Card Number (Should Be 16 Digits): "; cin >> cardNumber;
        if (cardNumber.length() != 16) {
            cout << "Invalid Card Number. Must be 16 digits.\n";
            continue;
        }

        cout << "Enter Expiry Date (MM/YY): "; cin >> expiry;
        if (expiry.length() != 5 || expiry[2] != '/') {
            cout << "Invalid Expiry Format. Use MM/YY.\n";
            continue;
        }
        cout << "Enter CVV: "; cin >> cvv;
        if (cvv.length() != 3) {
            cout << "Invalid CVV. Must be 3 digits.\n";
            continue;
        }

        string otp;
        cout << "Enter OTP sent to your registered number: "; cin >> otp;

        //Composition
        if(payment)delete payment;
        payment=new Payment<double>(amount,paymentMethod);
        payment->processPayment();


        if (paymentChoice == 1) {
            method = "Credit Card";
        } else if (paymentChoice == 2) {
           method = "Debit Card";
        } else if (paymentChoice == 3) {
            method = "PayPal";
        }


        return true;
    }
    cout<<"Payment Failed \n";
    return false;

}

//===========================================ADMIN CLASS FUNCTIONS=======================================


void Admin::manageFlights(FlightBooking& fb) {

    int choice;
    do {
        cout << "1. Add Airline\n2. Add Flight\n3. View Flights\n4. View Payments \n0. Exit\nChoice: ";
        cin >> choice;
        if (choice == 1) {
            string airlineName;
            cin.ignore();
            cout << "Enter Airline Name: "; getline(cin, airlineName);
            fb.addAirline(airlineName);
        } else if (choice == 2) {
            string id, from, to, airline, type,dep ,arr,date;
            double price;
            int seats;
            cout << "Enter Flight ID: "; cin >> id;
            cout << "Enter From: "; cin >> from;
            cout << "Enter To: "; cin >> to;
            cout << "Enter Price: "; cin >> price;
            cout << "Enter Total Seats: "; cin >> seats;
            cout<<"";
            cin.ignore();
            cout << "Enter Airline Name: "; getline(cin, airline);
            cout << "Enter Flight Type (Domestic/International): ";
            getline(cin, type);
            cout<<"Enter Date (YYYY-MM-DD): ";     getline(cin,date);
            cout<<"Enter Flight Departure Time: "; getline(cin, dep);
            cout << "Enter Flight Arrival Time: "; getline(cin, arr);
            fb.addFlight(id, from, to, price, seats, airline, type ,dep ,arr,date);
            fb.saveToFile("flights.json");
        } else if (choice == 3) {
            fb.viewFlights();
        }
        else if(choice==4) {
            string airline;
            vector<string> airlines = fb.getAllAirlineNames();
            if (airlines.empty()) {
                cout << "No airlines available.\n";
                break;
            }

            cout << "\nAvailable Airlines:\n";
            for (const auto& name : airlines) {
                cout << "- " << name << "\n";
            }
            cin.ignore();
            cout<<"Enter Airline Name: ";
            getline(cin,airline);
            fb.displayPaymentsForAirline(airline);
        }
        else if(choice>4 || choice<0) {
          cout << "Invalid Choice.\n";
        }

    } while (choice != 0);
}
//---------------------------------------------------------------------------------------------------------




//======================================PAYMENT CLASS FUNCTIONS=============================================

//All Defined in Functions.h As it is Template class


//=========================================FLIGHT BOOKING CLASS FUNCTIONS=========================================


Flight* FlightBooking::findFlight(string flightID) {
    for (Flight* f : flights) {
        if (f->getFlightID() == flightID) return f;
    }
    cout<<"No Such Flight Exist";
    return nullptr;
}

//------------------------------------------------------------------------------------------------------------------

void FlightBooking::viewFlights() const {
    if (flights.empty()) {
        cout << "No flights available.\n";
        return;
    }
    //Get the current date
    time_t t = time(0);
    tm* now = localtime(&t);
    char currentDate[11]; // YYYY-MM-DD format
    strftime(currentDate, sizeof(currentDate), "%Y-%m-%d", now);
    cout<<"\n ========================== Available Flights ======================== \n";
    for (Flight* f : flights) {
        if(f->getSchedule().getDate()>=currentDate) {
            f->display();
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------

void FlightBooking::addFlight(string id, string from, string to, double price, int seats, string airlineName, string type,string dpeTime,string arrivalTime,string date) {
    for (auto a : airlines) {
        if (a->getAirlineName() == airlineName) {

            if (type == "Domestic")
                flights.push_back(new DomesticFlight(id, from, to, price, seats, a ,dpeTime, arrivalTime,date));
            else if(type == "International"){
                flights.push_back(new InternationalFlight(id, from, to, price, seats, a,dpeTime, arrivalTime,date));
         //   cout << "Flight " << id << " added successfully.\n";
                }
            else {
                cout<<"No such Flight Exist";
            }
            return;
        }
    }
    cout << "Airline not found! Flight not added.\n";
}

//-------------------------------------------------------------------------------------------------------------------

void FlightBooking::addAirline(string name) {
    for(auto a: airlines) {
        if(a->getAirlineName()==name) {
            cout<<"Airline Already Exist! \n";
            return;
        }
    }
    airlines.push_back(new Airline(name));
  //  cout << "Airline " << name << " added successfully.\n";
}

//-------------------------------------------------------------------------------------------------------------------

void FlightBooking::saveToFile(string filename) {
    json j;  // Create a JSON object
    for (Airline* a : airlines) {
        j["airlines"].push_back(a->getAirlineName());
    }
    for (Flight* f : flights) {

        json flightObj;  // Create a separate JSON object
        flightObj["FlightID"] = f->getFlightID();
        flightObj["From"] = f->getFrom();
        flightObj["To"] = f->getDestination();
        flightObj["Price"] = f->getPrice();
        flightObj["TotalSeats"] = f->getTotalSeats();
        flightObj["BookedSeats"] = f->getBookedSeats();
        flightObj["Airline"] = f->airline->getAirlineName();
        flightObj["Type"] = dynamic_cast<DomesticFlight*>(f) ? "Domestic" : "International";
        flightObj["Departure"]=f->getSchedule().getDepartureTime();
        flightObj["Arrival"]=f->getSchedule().getArrivalTime();
        flightObj["Date"] = f->getSchedule().getDate();
        j["flights"].push_back(flightObj);  // Append flight details to JSON array
    }

    ofstream outFile(filename);
    if (!outFile) {
        cout << "Error: Unable to open file for writing.\n";
        return;
    }

    outFile << setw(4)<<j; // Pretty print JSON with indentation
    outFile.close();

    //cout << "Flight data saved to " << filename << " successfully!\n";
}

//-------------------------------------------------------------------------------------------------------------------

void FlightBooking::loadFromFile(string filename) {
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        cout << "No existing flight data found.\n";
        return;
    }



    time_t t = time(0);
    tm* now = localtime(&t);
    char currentDate[11]; // YYYY-MM-DD format
    strftime(currentDate, sizeof(currentDate), "%Y-%m-%d", now);




    json flightData;
    inFile >> flightData;  // Read and parse JSON data
    inFile.close();
  if (flightData.contains("airlines")) {
        for (const auto& airlineName : flightData["airlines"]) {
            addAirline(airlineName);
        }
    }
    for (const auto& f : flightData["flights"]) {
        string flightID = f["FlightID"];
        string from = f["From"];
        string to = f["To"];
        double price = f["Price"];
        int totalSeats = f["TotalSeats"];
        int bookedSeats = f["BookedSeats"];
        string airlineName = f["Airline"];
        string type = f["Type"];
        string departure = f["Departure"];
        string arrival = f["Arrival"];
        string date = f["Date"];
        // Add loaded flight to the system
        if(date>=currentDate) {
            addFlight(flightID, from, to, price, totalSeats, airlineName , type ,departure, arrival ,date);

            // Manually update bookedSeats since addFlight assumes new flights
            flights.back()->bookSeat(bookedSeats);
        }
    }

   // cout << "Flight data loaded successfully from " << filename << "!\n";
}

//--------------------------------------------------------------------------------------------------------------

void FlightBooking::displayPaymentsForAirline(const string& airlineName) const {
    auto it = airlinePaymentDetails.find(airlineName);
    if (it == airlinePaymentDetails.end() || it->second.empty()) {
        cout << "No payments found for airline: " << airlineName << "\n";
        return;
    }

    cout << "\n===== PAYMENTS FOR " << airlineName << " =====\n";
    double total = 0.0;
    for (const auto& payment : it->second) {
        cout << "Flight ID: " << payment.flightID
             << " | Amount: $" << fixed << setprecision(2) << payment.amount
             << " | Date: " << payment.date
             << " | Payment Method: " << payment.method << endl;
        total += payment.amount;
    }
    cout << "----------------------------\n";
    cout << "Total Revenue: $" << total << "\n";
}

//-----------------------------------------------------------------------------------------------------------------

void FlightBooking::savePaymentsToFile(const std::string& filename) {
    json j;
    for (const auto& entry : airlinePaymentDetails) {
        json paymentList;
        for (const auto& payment : entry.second) {
            paymentList.push_back(payment.toJson());
        }
        j["payments"][entry.first] = paymentList;
    }

    std::ofstream outFile(filename);
    if (outFile) {
        outFile << std::setw(4) << j;
    }
}

//-------------------------------------------------------------------------------------------------------------

void FlightBooking::loadPaymentsFromFile(const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile) return;

    json j;
    inFile >> j;

    if (j.contains("payments")) {
        for (auto& l : j["payments"].items()) {
            for (const auto& item : l.value()) {
                airlinePaymentDetails[l.key()].push_back(PaymentRecord::fromJson(item));
            }
        }
    }
}

//--------------------------------------------------------------------------------------------------------------

void FlightBooking::recordPayment(Flight* flight, double amount,string method) {
    PaymentRecord record;
    record.flightID = flight->getFlightID();
    record.airlineName = flight->getAirline()->getAirlineName();
    record.amount = amount;
    record.date = PaymentRecord::getCurrentDate();
    record.method=method;
    airlinePaymentDetails[record.airlineName].push_back(record);
}

//--------------------------------------------------------------------------------------------------------------

vector<string> FlightBooking::getAllAirlineNames() const {
    vector<string> names;
    for (const auto& airline : airlines) {
        names.push_back(airline->getAirlineName());
    }
    return names;
}

//================================================FLIGHT CLASS FUNCTIONS=========================================


bool Flight::bookSeat(int num) {
    if (bookedSeats + num <= totalSeats) {
        bookedSeats += num;
        return true;
    }
    cout<<"Booking Failed \n";
    return false;
}

//-------------------------------------------------------------------------------------------------------------------

void Flight::cancelSeat(int num) {
    if (num <= bookedSeats) bookedSeats -= num;
}

//-------------------------------------------------------------------------------------------------------------------

void Flight::display() const {
    cout <<"\n" <<"\n" << "Flight ID: " << flightID << " | From: " << from << " | To: " << destination
         << " | Price: " <<fixed<<setprecision(0) <<price << " | Available Seats: " << (totalSeats - bookedSeats)
         << " | Airline: " << airline->getAirlineName() <<" | ";schedule.displaySchedule();cout<< endl<<"\n" <<"\n"  ;
}
//==========================================AIRLINE CLASS FUNCTIONS===========================================

void Airline::setAirlineName(string name) { airlineName = name; }

//=========================================PAYMENTS RECORD CLASS FUNCTIONS======================================

json PaymentRecord::toJson() const {
    return {
                    {"flightID", flightID},
                    {"airline", airlineName},
                    {"amount", amount},
                    {"date", date},
                    {"method",method}
    };
}

PaymentRecord PaymentRecord::fromJson(const json& j) {
    PaymentRecord record;
    record.flightID = j["flightID"];
    record.airlineName = j["airline"];
    record.amount = j["amount"];
    record.date = j["date"];
    record.method=j["method"];
    return record;
}

 string PaymentRecord::getCurrentDate() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    tm tm_buf;
    localtime_r(&in_time_t, &tm_buf);
    stringstream ss;
    ss << put_time(&tm_buf, "%Y-%m-%d");
    return ss.str();
}

//============================================OPERATOR OVERLOADING============================================


ostream& operator<<(ostream& os, const Flight& flight) {
   // double totalPrice = flight.price * flight.getLastBookedSeats();

    os<<"\n";
    os<<"\n" ;
    os << "=============================================\n";
    os << "|                FLIGHT TICKET              |\n";
    os << "=============================================\n";
    os << "| " << left << setw(14) << "Airline"    << " : " << setw(25) << flight.airline->getAirlineName() << "|\n";
    os << "| " << left << setw(14) << "Flight ID"  << " : " << setw(25) << flight.flightID << "|\n";
    os << "| " << left << setw(14) << "Date" << " : " << setw(25) << flight.schedule.getDate() << "|\n";
    os << "| " << left << setw(14) << "From"       << " : " << setw(25) << flight.from << "|\n";
    os << "| " << left << setw(14) << "To"         << " : " << setw(25) << flight.destination << "|\n";
    //os << "| " << left << setw(14) << "Price"      << " : $" << setw(24) << fixed << setprecision(2) << totalPrice << "|\n";
    os << "| " << left << setw(14) << "Departure Time"<< " : " << setw(25) << flight.schedule.getDepartureTime() << "|\n";
    os << "| " << left << setw(14) << "Arrival Time" << " : " << setw(25) << flight.schedule.getArrivalTime() << "|\n";
    os << "=============================================\n";
    os<<"\n" ;
    os<<"\n" ;
    return os;
}

//==============================================================================================================


FlightBooking* FlightBooking::instance = nullptr;

