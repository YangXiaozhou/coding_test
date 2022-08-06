#include <iostream>
#include <vector>
#include <sstream>
#include <unordered_map>
using namespace std;

// Function protypes
void getRowDataVector(char single_row_data_str[], vector<string> &single_row_data_vec); 
unsigned long long int getTimeGap(unsigned long long int time1, unsigned long long int time2);
unsigned long long int getTimestamp(vector<string> single_row_data_vec, int TIMESTAMP_IDX);
string getSymbol(vector<string> single_row_data_vec, int TIMESTAMP_IDX);
unsigned long long int getPrice(vector<string> single_row_data_vec, int TIMESTAMP_IDX);
unsigned long long int getQuantity(vector<string> single_row_data_vec, int TIMESTAMP_IDX);
void updateMaxTimeGap(
	int counter, 
	unsigned long long int timestamp,
	unsigned long long int &max_time_gap,
	unsigned long long int &prev_timestamp
	);
void updateTotalVolume(
	unsigned long long int quantity, 
	unsigned long long int &total_volume
	);
void updateMaxPrice(
	unsigned long long int price, 
	unsigned long long int &max_price
	);
void updateTotalCost(
	unsigned long long int price, 
	unsigned long long int quantity, 
	unsigned long long int &total_cost
	);


int main(int argc, char** argv) {

	// char input_filename[] = argv[1];
	// char output_filename[] = argv[2];
	#ifndef ONLINE_JUDGE
		freopen(argv[1], "r", stdin);  // redirects standard input
		freopen(argv[2], "w", stdout);  // redirects standard output
	#endif

	// Initiate index of different variables in rows of the input file
	static unsigned char TIMESTAMP_IDX = 0;
	static unsigned char SYMBOL_IDX = 1;
	static unsigned char QUANTITY_IDX = 2;
	static unsigned char PRICE_IDX = 3;

	// Define constants
	static unsigned char MAX_SPACES = 100;
	static unsigned char RESERVE_SPACES = 10;
	static string COUNTER = "counter";
	static string MAX_TIME_GAP = "max_time_gap";
	static string PREV_TIMESTAMP = "prev_timestamp";
	static string TOTAL_VOLUME = "total_volume";
	static string MAX_PRICE = "max_price";
	static string TOTAL_COST = "total_cost";
	static string WEIGHTED_AVERAGE_PRICE = "weighted_average_price";


	// Initialize variables
	char single_row_data_str[MAX_SPACES];    // char array to store raw string of data
	vector<string> single_row_data_vec; // vector to store the data in each row
	single_row_data_vec.reserve(RESERVE_SPACES); // reserve space for 5 elements in the vector; 

	unsigned long long int timestamp;
	string symbol;
	unsigned long long int quantity;  
	unsigned long long int price; 
	unsigned long long int weighted_average_price = 0;

	// Vector of maps to hold all records
	unordered_map<string, unordered_map<string, unsigned long long int> > all_records;
	// Vector of symbols to hold all symbols
	vector<string> all_symbols;
	

	while (cin.getline(single_row_data_str, MAX_SPACES)) {  // read each line of the input file

		// Get current row of data as a vector
		getRowDataVector(single_row_data_str, single_row_data_vec);

		// Parse data into different variables
		timestamp = getTimestamp(single_row_data_vec, TIMESTAMP_IDX);
		symbol = getSymbol(single_row_data_vec, SYMBOL_IDX);
		price = getPrice(single_row_data_vec, PRICE_IDX);
		quantity = getQuantity(single_row_data_vec, QUANTITY_IDX);

		// Check if a symbol is already in the map
		if (all_records.find(symbol) == all_records.end()) {
			// If not, create a new map for the symbol
			unordered_map<string, unsigned long long int> new_trade;
			new_trade[COUNTER] = 1;
			new_trade[MAX_TIME_GAP] = 0;
			new_trade[PREV_TIMESTAMP] = timestamp;
			new_trade[TOTAL_VOLUME] = quantity;
			new_trade[MAX_PRICE] = price;
			new_trade[TOTAL_COST] = price * quantity;
			all_records[symbol] = new_trade;
			all_symbols.push_back(symbol);

		} else {
			// If yes, update the existing map
			all_records[symbol][COUNTER]++;

			// Update max time gap
			updateMaxTimeGap(
				all_records[symbol][COUNTER], 
				timestamp, 
				all_records[symbol][MAX_TIME_GAP], 
				all_records[symbol][PREV_TIMESTAMP]
				);

			// Update total volume
			updateTotalVolume(quantity, all_records[symbol][TOTAL_VOLUME]);
			
			// Update max price
			updateMaxPrice(price, all_records[symbol][MAX_PRICE]);

			// Update total cost
			updateTotalCost(price, quantity, all_records[symbol][TOTAL_COST]);
 
		}

		// Clear the vector for the next row
		single_row_data_vec.clear();
		
	}

	// Sort the symbols in alphabetical order
	sort(all_symbols.begin(), all_symbols.end());

	// Print all records according to the sorted symbols
	// Output format: <symbol>,<MaxTimeGap>,<Volume>,<WeightedAveragePrice>,<MaxPrice> 
	for (int i = 0; i < all_symbols.size(); i++) {
		all_records[all_symbols[i]][WEIGHTED_AVERAGE_PRICE] = all_records[all_symbols[i]][TOTAL_COST] / all_records[all_symbols[i]][TOTAL_VOLUME];
		
		cout << all_symbols[i]; 
		cout << "," << all_records[all_symbols[i]][MAX_TIME_GAP];
		cout << "," << all_records[all_symbols[i]][TOTAL_VOLUME]; 
		cout << "," << all_records[all_symbols[i]][WEIGHTED_AVERAGE_PRICE]; 
		cout << "," << all_records[all_symbols[i]][MAX_PRICE] << endl;
	}

	return 0;   
}


// Function to get the time gap between two trades in microseconds
unsigned long long int getTimeGap(unsigned long long int time1, unsigned long long int time2)
{
	return time2 - time1;
}

// Function to get the timestamp of a trade
unsigned long long int getTimestamp(vector<string> single_row_data_vec, int TIMESTAMP_IDX)
{
	return stoull(single_row_data_vec[TIMESTAMP_IDX]);
}

// Function to get the symbol of a trade
string getSymbol(vector<string> single_row_data_vec, int SYMBOL_IDX)
{
	string symbol = single_row_data_vec[SYMBOL_IDX];

	// Check if symbol is 3-lettered
	if (symbol.size() != 3) {
		cout << "Error: Symbol is not 3-lettered" << endl;
		exit(1);
	} else {
		return symbol;
	}

}

// Function to get the price of a trade
unsigned long long int getPrice(vector<string> single_row_data_vec, int PRICE_IDX)
{
	return stoull(single_row_data_vec[PRICE_IDX]);
}

// Function to get the quantity of a trade
unsigned long long int getQuantity(vector<string> single_row_data_vec, int QUANTITY_IDX)
{
	return stoull(single_row_data_vec[QUANTITY_IDX]);
}

// Function to get the data from a string and store it in a vector
void getRowDataVector(char single_row_data_str[], vector<string> &single_row_data_vec)
{
	stringstream ss(single_row_data_str);  // create a stringstream object to parse the string
	while (ss.good()) {  // read each word in the string
		string substr;  
		getline(ss, substr, ',');  
		single_row_data_vec.push_back(substr);  // store the word in the vector
	}
}

// Function to update the maximum time gap between trades
void updateMaxTimeGap(
	int counter, 
	unsigned long long int timestamp,
	unsigned long long int &max_time_gap,
	unsigned long long int &prev_timestamp
	)
{
	if (counter == 1) {  // if this is the first row, store the timestamp in prev_timestamp
		max_time_gap = 0;
	} else {  // if this is not the first row, calculate the time gap and store it in max_time_gap
		unsigned long long int time_gap = getTimeGap(prev_timestamp, timestamp);
		if (time_gap > max_time_gap) {  // if the time gap is greater than the current max time gap, store it in max_time_gap
			max_time_gap = time_gap;
		}
	}

	prev_timestamp = timestamp;  // update the previous timestamp

}

// Function to update the total volume of the symbol
void updateTotalVolume(unsigned long long int quantity, unsigned long long int &total_volume)
{
	total_volume += quantity;
}

// Function to update the maximum price of the symbol
void updateMaxPrice(unsigned long long int price, unsigned long long int &max_price)
{
	if (price > max_price) {
		max_price = price;
	}
}

// Function to update the total cost of the symbol
void updateTotalCost(unsigned long long int price, unsigned long long int quantity, unsigned long long int &total_cost)
{
	total_cost += price * quantity;
}
