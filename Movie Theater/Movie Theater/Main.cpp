#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <thread> 
#include <chrono> 

using namespace std;

/***************************************************************
Problem: Theater Seating
Question: Write a program that can be used by a small theater to
sell tickets for performances.

Name: Royce Kim
ID: 1559541
Date: 09/30/2018
Status: complete
****************************************************************/

void replaceSeats(string &newSeats, const string &initialSeats) {
    int lowerSeatMax = ((newSeats.size() < initialSeats.size() )?
                        newSeats.size() : initialSeats.size());

    for (int i = 0; i < lowerSeatMax; i++) {
        newSeats.at(i) = initialSeats.at(i);
    }
    
}

void clearTheater(vector<string> &chosen_vector) {
    chosen_vector.clear();
}

int countTix(const vector<string> &theater) {
    int tix = 0;
    for (string row : theater) {
        for (char seat : row) {
            tix += seat == '*';
        }
    }
    return tix;
}

void display(const vector<string> &vect, int seatSize, vector<string> prices){
    string firstLine = "";
    string temp;
    stringstream ss;

    cout << endl;
    cout << setw(22) << right << "Seats" << endl;

    for (int i = 0; i < seatSize; i++) {    //display seats
        ss << ((i + 1) % 10);
        ss >> temp;
        firstLine += temp;
        ss.clear();
    }
    cout << setw(seatSize + 8) << right << firstLine << endl;

    for (int i = 0; i < vect.size(); i++) {   //row count
        ss << (i + 1);
        ss >> temp;
        temp = "row " + temp;
        cout << setw(7) << left << temp << " " << setw(seatSize) <<
            left << vect.at(i) << "--$" << prices.at(i) << endl;
        ss.clear();
    }
    cout << "* = special rows" << endl;

}

bool strIsNum(const string &checkStr) { //check if input is number
    return checkStr.find_first_not_of(".0123456789") == string::npos;
}

bool splitString(const string &originalString, string &target1, 
                 string &target2, string delimeter = "S") {

    bool isPassed = false;
    int sPos;
    string testTar1;
    string testTar2;

    sPos = originalString.find(delimeter);

    testTar1 = originalString.substr(1, sPos - 1);
    testTar2 = originalString.substr(sPos + 1, 
                                     originalString.size() - sPos);


    if (((toupper(originalString.at(0)) == 'R') && sPos) &&
        (strIsNum(testTar1) && strIsNum(testTar2))) {

        isPassed = true;
        target1 = testTar1;
        target2 = testTar2;
    }

    return isPassed;
}

double calculateTotal(const vector<string> &theater, 
                      int seatMax, const vector<string> &prices) {

    double total = 0;
    double price = 0;
    int sPos;
    int count = 0;
    string buffer;

    for (int i = 0; i < theater.size(); i++) {//search each row
        sPos = prices.at(i).find("*");
        stringstream(prices.at(i).substr(0,sPos)) >> price; 

        for (char seat : theater.at(i)) {
            count += (seat == '*');
        }
        total += (count * price);
        count = 0;
    }
    return total;
}

void load(const string &fileName, vector<string> &destinationVector, 
          int &rowMax, int &seatsMax, vector<string> &rowPrices) {

    ifstream inputFile(fileName);
    string str;
    string row;
    string price;

    clearTheater(destinationVector);
    if (inputFile) {
        while (getline(inputFile, str)) {
            //split price from actual row saves
            stringstream(str) >> price >> row;  

            destinationVector.push_back(row);
            rowPrices.push_back(price);
        }
        seatsMax = destinationVector.at(0).size();
        rowMax = destinationVector.size();
    }

    cout << "Settings Imported." << endl;
    inputFile.close();
}

void save(const vector<string> &theater, const string &fileName,
          vector<string> &rowPrices) {
    ofstream outputFile(fileName);

    if (outputFile) {
        for (int i = 0; i < theater.size(); i++) {
            outputFile << rowPrices.at(i) << " ";
            outputFile << theater.at(i) << endl;
        }

    }

    cout << "Exported." << endl;
}

vector<string> initializeTheater(int rowCount, int seats) {
    string row = "";

    for (int i = 0; i < seats; i++) {
        row += "#";
    }

    vector<string> theater(rowCount, row); //15 rows 
    return theater;
}

//1.purchase menu functions:

//find first available seat
string findSeats(const vector<string> &originRows, int originSeats, 
                 int numberOfSeats, int startingRow = 0) { 
    string foundSeat = "none";
    string temp;
    string searchSeat = "";
    stringstream ss;
    bool seatFound = false;
    int foundPos;

    //generate searchable string
    for (int i = 0; i < numberOfSeats; i++) { 
        searchSeat += "#";
    }

    //find a row that it exists
    for (int r = startingRow; r < originRows.size() && !seatFound; r++) { 
        foundPos = originRows.at(r).find(searchSeat);

        if (foundPos < originSeats && foundPos >= 0) {
            foundSeat = "R";
            ss << (r + 1);
            ss >> temp;

            foundSeat += temp;
            ss.clear();

            ss << (foundPos + 1);
            ss >> temp;

            foundSeat += ("S" + temp);
            seatFound = true;
        }
    }

    return foundSeat;
}

bool seatIsThere(const vector<string> &originRows, int originSeats, 
                 int inRows, int inSeats) { //check if seat is available
    char seatChar;
    bool isAvailable = false;

    if ((inRows <= originRows.size()) || (inSeats <= originSeats)) {
        isAvailable = (originRows.at(inRows-1).at(inSeats-1) == '#');
    }
    return isAvailable;
}


void buySeat(vector<string> &inputVector, int maxSeats, const string &userInput,
             int numberOfSeats = 1) { //replace # with *

    int sPos = userInput.find("S");
    int seatNum;
    int rowNum;
    string seatChars = "";
    string seatString = userInput.substr(sPos + 1, userInput.size() - sPos);
    string rowString = userInput.substr(1, sPos - 1);

    for (int i = 0; i < numberOfSeats; i++) {
        seatChars += "*";
    }

    //split input into R, rowNum, S, seatNum
    if (userInput.at(0) == 'R' && sPos &&
        (strIsNum(rowString) && strIsNum(seatString)) ) {

        stringstream(rowString) >> rowNum; //find row

        stringstream(seatString) >> seatNum;  //find seat

         //if seat is available
        if (seatIsThere(inputVector, maxSeats, rowNum, seatNum)) {
            inputVector.at(rowNum - 1).replace(seatNum - 1,
                                               seatChars.size(), seatChars);
            cout << "Seat Purchased!" << endl;
        }
    }
    else { //if no seat found/taken
        cout << "No such seat or the seat has been taken."<< endl << endl;
        this_thread::sleep_for(chrono::seconds(2));
    }
} 

//functions for the purchase menu
void purchaseMenu(vector<string> &theater, 
                  int maxSeats,
                  vector<string> &rowPrices,
                  const string &fileName,
                  int &ticketsSold) { 
    int userCommand = 1;
    int requestedSeats = 1;
    int rowStart;
    double ticketPrice = 11.50;
    string userInput;
    string foundSeats;
    string searchSeat = "";

    while (userCommand != 0) {
        system("cls");
        display(theater, maxSeats, rowPrices);

        cout << endl << "*--<Purchase Menu>--*" << endl <<
            "1. Buy a seat" << endl <<
            "2. Find a seat" << endl <<
            "0. Return to Main Menu" << endl;
        getline(cin, userInput);
        stringstream(userInput) >> userCommand;

        switch (userCommand) {
        case 1: //buy a single seat
            system("cls");
            display(theater, maxSeats, rowPrices);

            cout << "Enter seat in R#S# format (ie. R10S25): ";
            getline(cin, userInput);
            buySeat(theater, maxSeats, userInput);
            save(theater, fileName, rowPrices);
            break;
        case 2: //find a seat
            system("cls");
            display(theater, maxSeats, rowPrices);

            cout << "How many seats would you like?";
            getline(cin, userInput);
            stringstream(userInput) >> requestedSeats;

            cout << "From which row?";
            getline(cin, userInput);
            stringstream(userInput) >> rowStart;

            foundSeats = 
                findSeats(theater, maxSeats, requestedSeats, rowStart - 1);

            if (!foundSeats.compare("none")) {  //if no seats were found

                cout << "No seats found" << endl << endl;
                this_thread::sleep_for(chrono::seconds(2));
            }
            else {
                cout << "Seat is found at " << foundSeats << "." << endl <<
                    "Would you like to purchase it? [ Y(es) | N(o) ] ";
                getline(cin, userInput);

                if (toupper(userInput.at(0)) == 'Y') {
                    buySeat(theater, maxSeats, foundSeats, requestedSeats);
                    save(theater, fileName, rowPrices);
                }
                
            }
            break;
        case 0: //return to main menu
            break;//avoid default prompt
        default:
            cout << "Unknown command, please try again" << endl << endl;
            this_thread::sleep_for(chrono::seconds(2));
            break;
        }
    }
}

//2.Setup menu

void displayPrices(const vector<string> &priceList) {
    string temp;
    stringstream ss;
    
    cout << "Prices by Row:" << endl;
    
    for (int i = 0; i < priceList.size(); i++) {   //row count
        ss << (i + 1);
        ss >> temp;
        temp = "row " + temp;
        cout << setw(7) << left << temp << " $" << left << 
            priceList.at(i) << endl;
        ss.clear();
    }
}

vector<string> scaleTheater(const vector<string> &initialTheater, 
                            int currentSeatMax, int newRowMax, 
                            int newSeatMax) {

    vector<string> newTheater;
    int size;

    //fill with '*'
    newTheater = initializeTheater(newRowMax, newSeatMax); 

    size = (initialTheater.size() < newRowMax) ? 
            initialTheater.size() : newRowMax;

    for (int i = 0; i < size; i++) { //replace with original inputs
         replaceSeats(newTheater.at(i), initialTheater.at(i));
    }

    return newTheater;
}

void initializeSeatPrices(vector<string> &seatPrices,
                          double price,
                          int maxRows,
                          bool replaceOld = false) {

    int rowPos = 0;
    int sPos;
    string priceString = "";
    stringstream ss;
    
    ss << price;
    ss >> priceString;

    vector<string> tempPrices(maxRows, priceString);

    if (replaceOld) { //keep special prices
        for (int i = 0; i < maxRows; i++) {
            sPos = seatPrices.at(i).find("*");

            if (sPos > 0) {
                tempPrices.at(i) = seatPrices.at(i);
            }

        }
    }

    //sets maximum number of prices
    seatPrices = tempPrices;
}

vector<string> scalePrices(const vector<string> &originalPrices, 
                           int newRowMax, double defaultPrices) {
    vector<string> newPrices;
    int minSize = 0;
    int maxSize = 0;
    stringstream ss;
    string temp;

    ss << defaultPrices;
    ss >> temp;

    minSize =( (originalPrices.size() < newRowMax) ?
            originalPrices.size() : newRowMax );

    maxSize = ((originalPrices.size() > newRowMax) ?
               originalPrices.size() : newRowMax);
    
    newPrices = vector<string>(maxSize, temp);

    for (int i = 0; i < minSize; i++) {

        newPrices.at(i) = originalPrices.at(i);
    }

    return newPrices;
}


string specialSeatPrices(vector<string> &rowPrices, 
                         const string &userInput,
                         int maxRows ) {

    int rowNum;
    string token;
    string rowString;
    string priceString;
    string invalidStrings = " ";
    stringstream ss(userInput);

    while (getline(ss, token, ' ')) {
        //checked if rowstring is num
        if (splitString(token, rowString, priceString, "$")) { 
            stringstream(rowString) >> rowNum;

            if ( (rowNum - 1) < rowPrices.size() ) {
                rowPrices.at((rowNum - 1)) = 
                    (priceString + "*");
            }
            else {
                invalidStrings += (token + " ");
            }
        }
        else {
            invalidStrings += (token + " ");
        }
    }

    return invalidStrings;
}

string blockMultiple(vector<string> &theater,
                     int seatMax,
                     stringstream ss) {

    string buffer;
    string rowString;
    string seatString;
    string invalidStrings = " ";
    int rowNum;
    int seatNum;
    int sPos;


    //split user input per space
    while (getline(ss, buffer, ' ') ) {
        sPos = buffer.find("S");

        //checked if rowstring is num
        if(splitString(buffer, rowString, seatString)){ 
            stringstream(rowString) >> rowNum;
            stringstream(seatString) >> seatNum;

            if ( (rowNum-1) < theater.size() ) {
                theater.at((rowNum - 1)).replace(
                                seatNum-1,1, "X");
            }
            else {
                invalidStrings += (buffer + "\n");
            }
        }
        else {
            invalidStrings += (buffer + "\n");
        }

    }
    return invalidStrings;

}

void setupWizard(vector<string> &theater, 
                 vector<string> &rowPrices,
                 int &currentMaxRows, 
                 int &currentMaxSeats) {

    vector<string> tempTheater;
    vector<string> tempPriceList;
    string userInput;
    string invalidStrings;
    int newSeatmax;
    int newRowMax;
    double requested;
    

    do {
        system("cls");
        cout << "*****<Setup Wizard>*****" << endl;
        cout << "*******************************" << endl;
        cout << "Please enter the number of rows: ";
        getline(cin, userInput);
    }
    while (!strIsNum(userInput));
    stringstream(userInput) >> newRowMax;   //set max rows

    do {
        system("cls");
        cout << "*****<Setup Wizard>*****" << endl;
        cout << "********************************" << endl;
        cout << "Please enter the number of seats: ";
        getline(cin, userInput);
    }
    while (!strIsNum(userInput));
    stringstream(userInput) >> newSeatmax;  //set max seats

    //started a new theater
    tempTheater = initializeTheater(newRowMax, newSeatmax);
    initializeSeatPrices(tempPriceList, 0, newRowMax);

    do {
        system("cls");
        cout << "*****<Setup Wizard>*****" << endl;
        display(tempTheater, newSeatmax, tempPriceList);

        cout << "********************************" << endl;
        cout << "What would be the default price? $";
        getline(cin, userInput);
    }
    while (!strIsNum(userInput));
    stringstream(userInput) >> requested;

    //initialize prices
    initializeSeatPrices(tempPriceList, requested, newRowMax); 

                                                                     
     //setup special seats
    do {
        system("cls");
        cout << "*****<Setup Wizard>*****" << endl;
        display(tempTheater, newSeatmax, tempPriceList);

        cout << (!(invalidStrings.compare(" ")) ? 
                 "" : "#Some inputs were invalid:") << endl <<
                 ((!invalidStrings.compare("") ? "" : invalidStrings)) <<
            endl;

        cout << "************************************" << endl;
        cout << "[Special Seats Set Up]" << endl;
        cout << "Enter a list of special rows and prices " <<
            "in R#$# format" << endl <<
            "(You may input multiple, spaces between each input)" << endl <<
            "ex: (R10$20 R2$21 R30$2)" << endl <<
            "press enter again to skip";

        if (getline(cin, userInput)) { //skip
            invalidStrings = specialSeatPrices(tempPriceList, 
                                               userInput,
                                               newRowMax);
        }

    }
    while (invalidStrings.compare(" "));

    //setup blocked seats

    invalidStrings = " ";
    do {
        system("cls");
        cout << "*****<Setup Wizard>*****" << endl;
        display(tempTheater, newSeatmax, tempPriceList);

        cout << (!(invalidStrings.compare(" ")) ? 
                 "" : "Some inputs were invalid:") << endl <<
                 ((!invalidStrings.compare("") ? "" : invalidStrings)) <<
            endl;

        cout << "************************************" << endl;
        cout << "[Blocked Seat Set up]" << endl;
        cout << "Enter a list of seats in R#S# format " <<
            "(You may input multiple, spaces between each input)" << endl <<
            "ex: (R10S20 R2S21 R30S2)";

        if (getline(cin, userInput)) {  //skip
            invalidStrings = blockMultiple(tempTheater, 
                                           newSeatmax, 
                                           stringstream(userInput));
        }

    }
    while (invalidStrings.compare(" "));

    system("cls");
    display(tempTheater, newSeatmax, tempPriceList);
    cout << "Would you like to save this list? Yes|No" << endl;
    getline(cin, userInput);

    if (toupper(userInput.at(0)) == 'Y') {
        theater = tempTheater;
        rowPrices = tempPriceList;
        currentMaxRows = tempTheater.size();
        currentMaxSeats = newSeatmax;
    }
}

void setupMenu(vector<string> &theater, int &currentMaxRows, 
               int &currentMaxSeats, vector<string> &rowPrices, 
               double DEFAULTROWPRICE) {

    string userInput;
    string invalidStrings = " ";
    vector<string> originalTheater = theater;
    vector<string> tempTheater;
    vector<string> tempPriceList;
    bool replaceCurrent = true;
    int originalMaxSeats = currentMaxSeats;
    int userCommand = 1;
    int requested;
    int delta = 0;
    int newRowMax = currentMaxRows;
    int newSeatmax = currentMaxSeats;
    double userDefaultPrice = 0.0;
    
    while (userCommand != 0) {
        system("cls");
        display(theater, currentMaxSeats, rowPrices);

        cout << endl << "}}--Set Up Menu--{{" << endl <<
            "1. Set up Wizard (Start a new Theater)" << endl <<
            "2. Change # of rows" << endl <<
            "3. Change # of seats per row" << endl <<
            "4. Change Row Prices" << endl <<
            "5. Add blocked seats" << endl <<
            "6. Clear current Theater" << endl <<
            "0. Return to Main Menu..." << endl;
        getline(cin, userInput);
        stringstream(userInput) >> userCommand;
 
        switch (userCommand) {
        case 1:
            setupWizard(theater, rowPrices,
                        currentMaxRows, currentMaxSeats);
            break;
        case 2:
        case 3:
            system("cls");
            display(theater, currentMaxSeats, rowPrices);

            cout << "WARNING: smaller sizes could delete" <<
                "special row prices!!!" << endl;//warning text

            cout << "The current max amount of " << 
                ((userCommand == 2) ? "rows" : "seats") <<
                " is " <<
                ((userCommand == 2) ? 
                 currentMaxRows : currentMaxSeats) <<
                "." << endl;

            cout << "What would you like to change it to?";
            getline(cin, userInput);
            stringstream(userInput) >> requested;

            if (userCommand == 2) {
                newRowMax = requested;
            }
            else {
                newSeatmax = requested;
            }
                

            theater = scaleTheater(theater,
                                   currentMaxSeats, 
                                   newRowMax,
                                   newSeatmax);

            currentMaxRows = newRowMax;
            currentMaxSeats = newSeatmax;

            
            rowPrices = scalePrices(rowPrices, currentMaxRows, DEFAULTROWPRICE);//clear
            system("cls");
            break;
        case 4: 
            system("cls");
            //price is displayed under seats
            displayPrices(rowPrices);   

            cout << "Please enter all row's default price: $";
            getline(cin, userInput);
            
            //if the default price was a number and number only
            if (strIsNum(userInput)) {  
                stringstream(userInput) >> userDefaultPrice;
                
                cout << "Replace current costs " <<
                    "(overwrite all special prices)? Yes | No >>";
                getline(cin, userInput);

                //should the current prices 
                //(and special rows) be replaced
                replaceCurrent = ( 
                    (toupper(userInput.at(0)) == 'N') );

                initializeSeatPrices(rowPrices,
                                     userDefaultPrice, 
                                     theater.size(),
                                     replaceCurrent);
                
                system("cls");
                displayPrices(rowPrices);   

                //let users know it has been completed.
                cout << "All seats now cost $" << 
                    userDefaultPrice << endl; 

                cout << "Would you like to add " <<
                    "special row prices? Yes | No >>";
                getline(cin, userInput);

                //add special row prices
                if (toupper(userInput.at(0)) == 'Y') {  
                    
                    cout << "Enter a list of special rows and" <<
                            " prices in R#$# format (You may " <<
                            "input multiple, spaces between " <<
                            "each input)" <<
                             endl << "ex: (R10$20 R2$21 R30$2)";
                    getline(cin, userInput);
                    
                    invalidStrings = specialSeatPrices(rowPrices,
                                                       userInput,
                                                       currentMaxRows);

                    cout << 
                        (!(invalidStrings.compare(" "))?
                         "" : "Some inputs were invalid:") <<
                        endl << ( (!invalidStrings.compare("") ?
                                   "" : invalidStrings) );
                }
            }
            else {
                cout << "Input invalid." << endl;
                this_thread::sleep_for(chrono::seconds(2));
            }
            
            system("cls");
            break;
        case 5:
            system("cls");
            display(theater, currentMaxSeats, rowPrices);

            cout << "Enter a list of seats in R#$# format "<<
                "(You may input multiple, spaces between each input)" <<
                endl << "ex: (R10S20 R2S21 R30S2)";
            getline(cin, userInput);
            
            blockMultiple(theater, currentMaxSeats, stringstream(userInput));
            break;

        case 6:
            cout << "Are you sure? Yes|No: ";
            getline(cin, userInput);

            if(toupper(userInput.at(0)) == 'Y'){
                clearTheater(theater);
            }
            break;
        case 0:
            system("cls");
            display(theater, currentMaxSeats, rowPrices);

            cout << "Would you like to save your changes? Yes|No :";
            getline(cin, userInput);

            if (toupper(userInput.at(0)) == 'Y') {
                save(theater, "setup.txt",rowPrices);
            }
            else {
                currentMaxSeats = originalMaxSeats;
                theater = originalTheater;
            }
            break;
        default:
            break;
        }
    }
}

//3. View availble seats
void viewAvailable(const vector<string> &theater,
                   const int &maxSeats, 
                   const vector<string> &priceList) {
    string temp;
    stringstream ss;
    int seatsTotal = 0;
    int count = 0;
    vector<int> availableSeatCount(theater.size());

    system("cls");

    //calculate available seats
    for (int i = 0; i < theater.size(); i++) {  
        for (char seat : theater.at(i)) {
            count += (seat == '#');
            seatsTotal += (seat == '#');

        }
        availableSeatCount.at(i) = count;
        count = 0;
    }

    for (int i = 0; i < theater.size(); i++) {
        ss << (i + 1);
        ss >> temp;
        temp = "row " + temp;

        cout << setw(7) << left << temp << " " <<
            left << availableSeatCount.at(i) <<
            " Seats" << endl;
        ss.clear();
    }
    cout << "Total Seats Available: " << seatsTotal << endl;
    system("pause");
}

//MAIN MENU
int main() {
    //initial 
    const string FILENAME("setup.txt");
    const double DEFAULTPRICE = 11.50;

    string userInput;
    double totalCost = 0.0;
    int seatsMax = 30;
    int rowsMax = 15;
    int ticketsSold = 0;
    int userCommand = 1;
    vector<string> theater;
    vector<string> rowPrices;   //special prices easier with strings
    ifstream fileInput(FILENAME);
    //--------

    if (fileInput) {//on success
                    
        // import at first
        load(FILENAME, theater, rowsMax, seatsMax, rowPrices);  
    }
    else {//else: initialize

        theater = initializeTheater(rowsMax, seatsMax);
        initializeSeatPrices(rowPrices, DEFAULTPRICE, rowsMax); 
        save(theater, "setup.txt", rowPrices);
    }
    cout << endl;

    //after loading
    while (userCommand != 0) {
        system("cls");
        display(theater, seatsMax, rowPrices);

        cout << endl;
        ticketsSold = countTix(theater);
        totalCost = calculateTotal(theater, seatsMax, rowPrices);

        cout << "Total Tickets Sold: " << ticketsSold << endl;
        cout << "TOTAL COST: $" << totalCost << endl;
        cout << "---<Main Menu>---" << endl <<
                "1. Purchase Tickets" << endl <<
                "2. Set Up" << endl <<
                "3. Veiw Available Seats" << endl <<
                "0. Exit" << endl;
         
        getline(cin, userInput);
        stringstream(userInput) >> userCommand;

        switch (userCommand) {
        case 1:
            system("cls");
            purchaseMenu(theater,seatsMax, rowPrices, FILENAME,
                         ticketsSold);
            userCommand = 1;
            break;
        case 2:
            system("cls");
            setupMenu(theater,rowsMax,seatsMax, rowPrices, DEFAULTPRICE);
            break;
        case 3:
            viewAvailable(theater, seatsMax, rowPrices);
            break;
        case 0://avoid default prompt
            break;
        default:
            cout << "Unknown command, please try again" << endl << endl;
            break;
        }
    }// end while

    return 0;
}
