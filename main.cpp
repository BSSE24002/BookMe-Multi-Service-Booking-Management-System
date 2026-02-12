#include "bsse24074.h"
#include "bsse24046.h"
#include "bsse24030.h"
#include "train.h"
#include "movie.h"
int main() {
    FlightBooking *flightBooking=FlightBooking::getInstance();
    Booking_Manager bookingManager;
    CarManager carManager;
    carManager.loadFromFile();
    bookingManager.loadFromFile(carManager);
    TravelRoute::loadRoutesFromFile();
    
    Admin_c* adminSystem = Admin_c::getInstance(bookingManager, carManager);
    Admin admin("admin", "1234");
    User user("user", "pass");
    string role;
    flightBooking->loadFromFile("flights.json");
    flightBooking->loadPaymentsFromFile("FlightsPayments.json");

    do{
        cout << "Are you logging in as Admin or User? (To Exit press 0) ";
        cin >> role;

        if (role == "Admin") {
            string username, password;
            cout << "Enter Admin Username: "; cin >> username;
            cout << "Enter Password: "; cin >> password;
            if (admin.login(username, password)) {
                int choice;
                do{

                    cout<<"In Which Module You Want To Make Changes? \n1. Manage Flights\n2. Manage Trains\n3. Manage Cars \n4. Manage Hotel \n5. Manage Movies \n0. Exit \nEnter Choice: \n";
                    cin>>choice;
                    if (choice == 1) {
                        admin.manageFlights(*flightBooking);
                    }
                    else if (choice == 2) {
                        trainSystemMenu(true);
                    }
                    else if (choice == 3) {
                        adminSystem->adminMenu();
                    }
                    else if (choice == 4) {
                        hotelBookingMenu(true);
                    }
                    else if (choice == 5) {
                        Cinema c;
                        c.adminLoginPage();
                    }
                    else if (choice > 5 || choice < 0) {
                        cout<<"Please Enter Valid Choice\n";
                    }
                } while (choice!=0);
            }
            else {
                cout << "Invalid Username or Password.\n";
            }
        } else if (role == "User") {
            int choice;
            do{

                cout<<"What Do You Want To Book? \n1. Flight Ticket\n2. Train Ticket\n3. Car Booking \n4. Hotel Room\n5. Movie Ticket \n0. Exit \nEnter Choice: \n";
                cin>>choice;
                if (choice == 1) {
                    user.accessFlights(*flightBooking);
                }
                else if (choice == 2) {
                    trainSystemMenu(false);
                }
                else if (choice == 3) {
                    string username;
                    cout << "Enter your username: ";
                    cin >> username;
                    User_c user(username, bookingManager, *adminSystem);
                    user.userMenu();
                }
                else if (choice == 4) {
                    hotelBookingMenu(false);
                }
                else if (choice == 5) {
                    Cinema c;
                    c.customerLoginPage();
                }
                else if (choice > 5 || choice < 0) {
                    cout<<"Please Enter Valid Choice\n";
                }
            } while (choice!=0);

        }
    }while(role!="0");

    flightBooking->saveToFile("flights.json");
    flightBooking->savePaymentsToFile("FlightsPayments.json");
    return 0;
}
