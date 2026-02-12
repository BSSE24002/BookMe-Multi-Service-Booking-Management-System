#include "movie.h"
#include <fstream>
#include <iomanip>
#include <algorithm>
using namespace std;

//This will maintain movie count and booking count
int MovieManager::getMovieCount()const{
    return (int)movies.size();
}
int BookingManager::getBookingCount()const{
    return (int)bookings.size();
}

//Using operator overloading to display the movie details
ostream& operator<<(ostream& os, const Movie& movie) {
    os << "Title: " << movie.title << "\n"
       << "Genre: " << movie.genre << "\n"
       << "Duration: " << movie.duration << " mins\n"
       << "Price: Rs " << movie.price << "\n"
       << "Rating: " << movie.rating << "/5\n"
       << "Schedule: " << movie.movieSchedule << "\n";
    return os;
}

//Parameterized constructor and destructor and virtual functions of class Account
Account::Account(string name_, string CNIC_, string password_, string role_) : name(name_), CNIC(CNIC_), password(password_), userRole(role_) {}
Account::~Account() {}

void Account::show(){
    cout << "Hi!" << endl;
 }

 //Setters and getters
string Account::getName()const{
    return name;
}
string Account::getCNIC()const{
    return CNIC;
}
string Account::getPassword()const{
    return password; 
}
string Account::getUserRole()const{
    return userRole; 
}
void Account::resetPassword(const string& newPass){
    password = newPass; 
}

//Function to store data on the json
json Account::toJson()const{
    return json{{"username", name}, {"password", password}, {"cnic", CNIC}, {"role", userRole}};
}

//Parameterized and default constructor of and overide function of Admin class
Adminm::Adminm(string name_, string CNIC_, string password_) : Account(name_, CNIC_, password_, "Admin"){}
Adminm::Adminm() : Account("", "", "", "Admin"){}
void Adminm::show(){ 
    cout << "Login successful!\nWelcome Admin" << endl;
 }

//Parameterized and default constructor of and overide function of Customer class
Customer::Customer(string name_, string CNIC_, string password_): Account(name_, CNIC_, password_, "Customer"){}
Customer::Customer() : Account("", "", "", "Customer"){}
void Customer::show(){ 
    cout << "Login successful!\nWelcome Customer" << endl;
 }

//Parameterized and default constructor of and overide function of Movie class
Movie::Movie() : duration(0), price(0), rating(0.0f){}
Movie::Movie(string t, string g, int d, int p, string ms, float r) : title(t), genre(g), duration(d), price(p), movieSchedule(ms), rating(r) {}

json Movie::toJson() const {
    return json{
        {"title", title},
        {"genre", genre},
        {"duration", duration},
        {"price", price},
        {"rating", rating},
        {"movieSchedule", movieSchedule}
    };
}

//This function will read data from the json file 
Movie Movie::fromJson(const json& j){
    return Movie(
        j.value("title", ""),
        j.value("genre", ""),
        j.value("duration", 0),
        j.value("price", 0),
        j.value("movieSchedule", ""),
        j.value("rating", 0.0f)
    );
}

//This will display movie details on the terminal
void Movie::display()const{
    cout << "Title: " << title << "\nGenre: " << genre << "\nDuration: " << duration << " mins\nPrice: Rs " << price << "\nRating: " << rating << "/5\nSchedule: " << movieSchedule << "\n\n";
}

MovieManager::MovieManager() : filename("movies.json"){
     loadFromFile(); 
    }

//This function will create instance of Movie Manager
MovieManager& MovieManager::getInstance(){
    static MovieManager instance;
    return instance;
}

//This will load data from json file 
void MovieManager::loadFromFile() {
    movies.clear();
    ifstream file(filename.c_str());
    if (!file.is_open()) return;
    json j;
    file>>j;
    file.close();

    for(auto& item : j){
        movies.push_back(Movie::fromJson(item));
    }
}

//this function will save data to the json file 
void MovieManager::saveToFile() {
    json j = json::array();
    for (auto& m : movies) {
        j.push_back(m.toJson());
    }
    ofstream file(filename.c_str());
    if (file.is_open()) {
        file << setw(4) << j << endl;
        file.close();
    }
}

//This function will help a new movie
void MovieManager::addMovie(){
    string title, genre, schedule;
    int duration, price;
    cin.ignore();
    cout << "Enter title: ";
    cin>>title;
    cout << "Enter genre: ";
    cin>>genre;
    do {
        cout << "Enter duration in Mins: ";
        cin >> duration;
        cin.ignore();
    } while (duration <= 0);
    do {
        cout << "Enter price in Rs: ";
        cin >> price;
        cin.ignore();
    } while (price < 0);
    cout << "Enter schedule: ";
    cin>>schedule;
    movies.push_back(Movie(title, genre, duration, price, schedule));
    saveToFile();
    cout << "Movie added.\n";
}


//This function will remove an existing movie by title
void MovieManager::removeMovie() {
    string title;
    cin.ignore();
    cout << "Enter title to remove: ";
    cin>>title;

    for (size_t i = 0; i < movies.size(); ++i) {
        if (movies[i].title == title) {
            movies.erase(movies.begin() + i);
            saveToFile();
            cout << "Movie removed.\n";
            return;
        }
    }
    cout<<"Movie not found.\n";
}

//This function will help to rate a specific movie by title
void MovieManager::rateMovie() {
    string title;
    float rating;
    cin.ignore();
    cout << "Enter movie title to rate: ";
    cin>>title;
    for(auto& movie : movies){
        if (movie.title == title) {
            do {
                cout<<"Enter rating (0 to 5): ";
                cin>>rating;
                cin.ignore();
            } while (rating < 0 || rating > 5);
            movie.rating = rating;
            saveToFile();
            cout << "Rating updated.\n";
            return;
        }
    }
    cout << "Movie not found.\n";
}

//This function will help to return movies lists
void MovieManager::viewMovies()const{
    if(movies.empty()){
        cout << "No movies available.\n";
        return;
    }
    for(const auto& movie : movies){
        movie.display();
    }
}

//This function will help to find the movie in the json file 
float MovieManager::findMoviePriceByTitle(const string& title){
  ifstream file(filename.c_str());
  if (!file.is_open()){
      cout << "Could not open the file.\n";
      return -1;  
  }
  json j;
  file>>j;
  file.close();

  string lowerTitle = title;
  std::transform(lowerTitle.begin(), lowerTitle.end(), lowerTitle.begin(), ::tolower);

  for(const auto& item : j){
      string movieTitle = item.value("title", "");
      
      string lowerMovieTitle = movieTitle;
      std::transform(lowerMovieTitle.begin(), lowerMovieTitle.end(), lowerMovieTitle.begin(), ::tolower);

      if (lowerMovieTitle == lowerTitle){
          return item.value("price", 0);
      }
  }
  cout << "Movie not found.\n";
  return -1;  
}


//Booking manager filename
BookingManager::BookingManager() : filename("Moviebookings.json"){}


//This function will help to make booking of a movie
void BookingManager::addBooking(const json& b){
    bookings.push_back(b);
}

//this function will load a booking from file
void BookingManager::loadFromFile(){
    bookings.clear();
    ifstream in(filename);
    if(!in.is_open()){
        return;
    }
    json j;
    in>>j;
    in.close();

    for (auto& item : j){
        bookings.push_back(item);
    }
}

//this function will save  a booking to the file
void BookingManager::saveToFile() {
    json j = bookings;
    ofstream out(filename);
    if (out.is_open()){
        out<<setw(4)<<j<<endl;
        out.close();
    }
}

//This function will show the bookings on the terminal of the specific user
void BookingManager::viewByPhoneNo(const string& phone) {
    bool found = false;
    for (const auto& b : bookings){
        if(b.value("PhoneNo", "") == phone){
            cout << "Movie: " << b.value("movie","") 
                 << ", Time: " << b.value("time","") 
                 << ", Category: " << b.value("Category","")<<endl;
            found = true;
        }
    }
    if (!found){
        cout << "No bookings found.\n";
    }
}

//this function will cancel all the bookings of a specific user
void BookingManager::cancelByPhoneNo(const string& phone) {
    bool found = false;
    for (auto it = bookings.begin(); it != bookings.end();){
        if(it->value("PhoneNo", "") == phone){
            it = bookings.erase(it);
            found = true;
        }
        else{
            ++it;
        }
    }
    if(found){
        saveToFile();
        cout << "Bookings canceled.\n";
    } 
    else{
        cout << "No bookings to cancel.\n";
    }
}

//Constrctor of the class customer booking
CustomerBooking::CustomerBooking(string phone_, BookingManager& bm) : phone(phone_), bookingManager(bm){}

//This function will book a movie mainly and use the helper functions in it
void CustomerBooking::bookMovie(MovieManager& mg) {
  string title, time, category;
  cout << "Enter movie title to book: ";
  cin>>title;

  float price = mg.findMoviePriceByTitle(title);
  if (price < 0) {
      cout << "Movie not found.\n";
      return;
  }
  cout<<"Booking movie: " << title << " at price: Rs " << price << endl;
  cout<<"Enter show time: ";
  cin>>time;
  cout<<"Enter category (Simple, Premium, Luxury): ";
  cin>>category;

if(category == "Simple"){
    price = price + 0;
}
else if(category == "Premium"){
  price = price + 500;
}
else if(category == "Luxury"){
  price = price + 1000;
}
if(category != "Simple" && category != "Premium" && category != "Luxury"){
    cout << "Invalid category.\n";
    return;
}
json b = {
    {"PhoneNo", phone},
    {"movie", title},
    {"Category", category},
    {"time", time}
};
bookingManager.addBooking(b);
bookingManager.saveToFile(); 
cout<<"Booking successful!\n";
}

//This function will calls the view booking function by object
void CustomerBooking::viewBookings(){
    bookingManager.viewByPhoneNo(phone);
}

//This function will calls the cancel booking function by object
void CustomerBooking::cancelBookings(){
    bookingManager.cancelByPhoneNo(phone);
}

AccountManager::AccountManager() : filename("Movieusers.json"){
    loadFromFile();
}

//this is the destructor of AccountManager class
AccountManager::~AccountManager(){
    for(auto u : users){
        delete u;
    }
}

//This function will return the user 
Account* AccountManager::findUser(const string& username, const string& role) {
    for (auto user : users){
        if(user->getName() == username && user->getUserRole() == role){
            return user;
        }
    }
    return nullptr;
}

//This function will load users from the file
void AccountManager::loadFromFile() {
    users.clear();
    ifstream in(filename);
    if(!in.is_open()){
        return;
    }
    json j;
    in>>j;
    in.close();
    for(auto& item : j){
        string name = item.value("username", "");
        string pass = item.value("password", "");
        string cnic = item.value("cnic", "");
        string role = item.value("role", "");
        if(role == "Admin"){
            users.push_back(new Adminm(name, cnic, pass));
        }
        else if(role == "Customer"){
            users.push_back(new Customer(name, cnic, pass));
        }
    }
}

//This function will save the data of the user to the file
void AccountManager::saveToFile() {
    json j = json::array();
    for(auto u : users){
        j.push_back(u->toJson());
    }
    ofstream out(filename);
    if(out.is_open()){
        out<<setw(4)<<j<<endl;
        out.close();
    }
}

//this function will register a new user 
void AccountManager::registerUser(string role) {
    string username, password, cnic;
    cout<<"Enter new username: ";
    cin>>username;
    if(findUser(username, role)){
        cout<<"Username already exists.\n";
        return;
    }
    cout<<"Enter CNIC (no dashes): ";
    cin>>cnic;
    cout<<"Enter password: ";
    cin>>password;
    Account* acc = nullptr;
    if(role == "Admin"){
        acc = new Adminm(username, cnic, password);
    }
    else if(role == "Customer"){
        acc = new Customer(username, cnic, password);
    }
    if(acc){
        users.push_back(acc);
        saveToFile();
        cout<<role<<" registered successfully.\n";
    }
}

//This function will work for both admin and customer login and match details and then give access
void AccountManager::loginUser(string role, Cinema& cinemaObj) {
    string username, password;
    cout<<"Enter username: ";
    cin>>username;
    cout<<"Enter password: ";
    cin>>password;
    Account* acc = findUser(username, role);
    if (!acc){
        cout<<"User not found.\n";
        return;
    }
    if (acc->getPassword() != password) {
        cout<<"Incorrect password.\n";
        return;
    }
    acc->show();
    cout<<" "<<username<<endl;
    if(role == "Admin"){
        cinemaObj.adminMenu();
    }
    else{
        cinemaObj.customerMenu();
    }
}

//This function will create a new password for the user if the previous password is forgotten
void AccountManager::forgotPassword(string role){
    string username, cnic;
    cout<<"Enter username: ";
    cin>>username;
    Account* acc = findUser(username, role);
    if (!acc){
        cout<<"User not found.\n";
        return;
    }
    cout<<"Enter CNIC to verify identity: ";
    cin>>cnic;
    if (acc->getCNIC() != cnic){
        cout<<"CNIC does not match.\n";
        return;
    }
    string newPass;
    cout<<"Enter new password: ";
    cin>>newPass;
    acc->resetPassword(newPass);
    saveToFile();
    cout<<"Password updated!\n";
}

void Cinema::adminLoginPage() {
    AccountManager am;
    int choice;
    while (true){
        cout << "========== ADMIN LOGIN ==========\n1. Login\n2. Register\n3. Forgot Password\n4. Change Text Color\n0. Back\nChoose: ";
        if (!(cin >> choice)){
            cin.clear();
            cin.ignore();
            cout<<"Invalid input!\n";
            continue;
        }
        cin.ignore();
        switch(choice){
            case 1: am.loginUser("Admin", *this); 
            break;
            case 2: am.registerUser("Admin"); 
            break;
            case 3: am.forgotPassword("Admin"); 
            break;
            case 4: colorChange();
            break; 
            case 0: return;
            default: cout<<"Invalid choice!\n"; 
            break;
        }
    }
}

void Cinema::customerLoginPage() {
    AccountManager am;
    int choice;
    while (true) {
        cout << "========== CUSTOMER LOGIN ==========\n1. Login\n2. Register\n3. Forgot Password\n4. Change Text Color\n0. Back\nChoose: ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore();
            cout<<"Invalid input!\n";
            continue;
        }
        cin.ignore();
        switch(choice){
            case 1: am.loginUser("Customer", *this); 
            break;
            case 2: am.registerUser("Customer"); 
            break;
            case 3: am.forgotPassword("Customer"); 
            break;
            case 4: colorChange();
            break;
            case 0: return;
            default: cout<<"Invalid choice!\n";
            break;
        }
    }
}

void Cinema::adminMenu() {
    MovieManager& mm = MovieManager::getInstance();
    BookingManager bm; // Create an instance of BookingManager
    Theater theater(mm, bm); // Pass the instance to Theater
    int choice;
    while(true){
        cout << "========== ADMIN MENU ==========\n1. View Movies\n2. Add Movie\n3. Remove Movie\n4. Show Theater Stats\n0. Back\nChoose: ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore();
            cout << "Invalid input!\n";
            continue;
        }
        cin.ignore();
        switch(choice){
            case 1: mm.viewMovies(); 
            break;
            case 2: mm.addMovie(); 
            break;
            case 3: mm.removeMovie(); 
            break;
            case 4: theater.showStats(); 
            break;
            case 0: return;
            default: cout<<"Invalid choice!\n"; 
            break;
        }
    }
}

void Cinema::customerMenu(){   //This function will provide customer menu options
    MovieManager& mm = MovieManager::getInstance();   //Singleton class instance
    BookingManager bm;
    bm.loadFromFile();

    cout<<"Enter your phone number: ";
    string phone;
    cin>>phone;

    CustomerBooking cb(phone, bm);  //Making obj of Customerbooking and passing value in it
    ListofMovieGenres<string> favoriteGenres;   

    int choice; 
    while (true) {
        cout << "========== CUSTOMER MENU ==========\n1. View Movies\n2. Book Movie\n3. View Bookings\n4. Cancel Booking\n5. Rate Movie\n6. Manage Favorite Genres\n0. Back\nChoose: ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore();
            cout << "Invalid input!\n";
            continue;
        }
        cin.ignore();
        switch(choice) {
            case 1: mm.viewMovies(); 
            break;
            case 2: cb.bookMovie(mm); 
            break;
            case 3: cb.viewBookings(); 
            break;
            case 4: cb.cancelBookings();
            break;
            case 5: mm.rateMovie();
            break;
            case 6: {
                cout << "1. Add Genre\n2. View Genres\n0. Back\nChoose: ";
                int subChoice; 
                cin>>subChoice; 
                cin.ignore();
                if(subChoice == 1) {
                   string genre;
                   cout << "Enter genre to add: ";
                   cin>>genre;
                   favoriteGenres.add(genre);
                } 
                  else if(subChoice == 2){
                      favoriteGenres.display();
                  }
            break;
        }
            case 0: 
            return;
            default: cout<<"Invalid choice!\n"; 
            break;
      }
    }
}

void Cinema::colorChange(){   //This function will change color
    int choice;
    do {
        cout << "========== CHANGE COLOR ==========\n1. Red\n2. Blue\n3. Green\n4. White\n0. Back\nChoose: ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore();
            cout << "Invalid input!\n";
            continue;
        }
        switch (choice) {
            case 1: setColor(31); 
            break;
            case 2: setColor(34); 
            break;
            case 3: setColor(32); 
            break;
            case 4: setColor(37); 
            break;
            case 0: cout << "Returning...\n"; 
            break;
            default: cout << "Invalid choice!\n"; 
            break;
        }
    } while (choice != 0);
}

void Cinema::setColor(int color) {   //This function will set the color accordingly
    cout<<"\033["<<color<< "m";
}