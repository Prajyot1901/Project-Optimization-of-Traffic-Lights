#include<iostream>
#include<queue>
#include<map>
#include<fstream>
#include<sstream>
using namespace std;
// To predict how many cars,HCVs and Two-Wheelers would arrive in interval of 10 mins for next hour
vector<int> predict_approach(string filename, string hour)
{
	fstream f;
	f.open(filename);
	if (f.fail())
	{
		cout << filename << " failed";
		exit;
	}
	string s;
	string h;
	int car_sum = 0, two_sum = 0, hcv_sum = 0;
	int car, two, hcv;
	int _cnt = 0;
	getline(f, s); // Copy the first line of the file(Column Names) and moving file pointer to next line;

	while (getline(f, s))  // Copy the line into string s and moving pointer to next line each time getline function runs;
	{
		stringstream ss(s);  //Converting the string into stringstream
		string cell;
		int cnt = 0;
		while (getline(ss, cell, ',')) // To tokenize the string s at ',' ;
		{
			cnt++;
			if (cnt == 2)
			{
				h = cell;
			}
			if (cnt == 3 )
			{
				if (cell != "")
					car = stoi(cell);
				else
					car = 0;
			}
			if (cnt == 4)
			{
				if (cell != "")
					two = stoi(cell);
				else
					car = 0;
				
			}
			if (cnt == 5)
			{
				if (cell != "")
					hcv = stoi(cell);
				else
					car = 0;
			}
		}
		if (h == hour)
		{
			_cnt++;
			car_sum += car;
			two_sum += two;
			hcv_sum += hcv;
		}
	}
	vector<int> ans;
	// Taking average of last 5 days as explained in report. 
	ans.push_back(car_sum / _cnt);       
	ans.push_back(two_sum / _cnt);
	ans.push_back(hcv_sum / _cnt);
	return ans;
}

// To combine the predicted values for the intersection
vector<vector<int>> predict(string hour)
{
	vector<vector<int>> ans;
	// Predict number of vehicles for different approachs, for particular approach. 
	ans.push_back(predict_approach("Raj_inputs.csv", hour));
	ans.push_back(predict_approach("Ulu_inputs.csv", hour));
	ans.push_back(predict_approach("ABC_inputs.csv", hour));
	return ans;
}

// To store the data in vector of string except the pre_hour of the first day
vector<string> read(string filename,string pre_hour)
{
	vector<string> ans;
	ifstream file;
	file.open(filename);
	if (file.fail())
	{
		cout << filename << " failed to open";
		exit;
	}
	string s;
	getline(file, s);
	ans.push_back(s);  // ans[0] = Contains the stub column of the file;
	while (getline(file,s))
	{
		string cell;
		stringstream ss(s);
		int cnt = 0;
		string hour;
		while (getline(ss, cell,','))
		{
			cnt++;
			if (cnt == 2)
			{
				hour = cell;
			}
		}
		if (hour != pre_hour)
		{
			ans.push_back(s); // ans[1] will be the row with first occurance of "hour" not equal to "h";
			break;
		}
	}
	while (getline(file, s))
	{
		ans.push_back(s);  // Push rest of all rows will be in "ans";
	}
	file.close();
	return ans;
}

// To write the updated data to the input file
void write(string filename, vector<string> v)
{
	ofstream file;
	file.open(filename);
	if (file.fail())
	{
		cout << filename << " failed to open";
		exit;
	}
	for (auto it : v)
	{
		file << it<<"\n";  // write every string in csv file; 
	}
	file.close();
}

// To estimate the waiting time of each approach
vector<int> myfun(int R_time, int U_time, int A_time,vector<vector<int>> v)
{
	// 10 minutes=600 seconds. This describes the time gap between two vehicles;
	float Raj_vol = 600.0 / ((v[0][0] + v[0][1] / 5 + v[0][2]) / 2); // Only half of the vehicles will stop at the signal since half of them are considered to turn left;
	float Ulu_vol = 600.0 / ((v[1][0] + v[1][1] / 5 + v[1][2]) / 5); // Here, only 20 percent vehicles will stop at the signal;
	float ABC_vol = 600.0 / (v[2][0] + v[2][1] / 5 + v[2][2]); // All the vehicles will stop at the signal untill the signal turns green;

	// To count the vehicle number that have already crossed the signal;
	int Raj_cnt = 0;
	int Ulu_cnt = 0;
	int ABC_cnt = 0;

	// The following is the average service rate of a single lane in each approach(0 if signal is red, 0.5 otherwise);
	float Ulu_service_rate = 0; 
	float ABC_service_rate = 0;
	float Raj_service_rate = 0.5;

	// The queue shows the queue of the vehicle at each approach and the integer value indicates the time when the vehicle has arrived;
	queue<int> Raj;
	queue<int> Ulu;
	queue<int> ABC;

	// counting the total waiting time of all the vehicles at specific approach;
	int raj_wait = 0; 
	int ulu_wait = 0;
	int abc_wait = 0;

	//The following three variables count the time since the last vehicle arrived at the particular approach;
	float Raj_index = 0;
	float ABC_index = 0;
	float Ulu_index = 0;

	// To count the time since the last vehicle has been served(passed the intersection);
	float Raj_ser_index = 0;
	float Ulu_ser_index = 0;
	float ABC_ser_index = 0;
	int v_cnt=0;
	for (int i = 0; i < 600; i++)
	{
		if (Raj_ser_index >= 1) // if the last vehicle was passed the intersection before 2 or more secs ;
		{
			if(!Raj.empty())
			{
				raj_wait = raj_wait + i - Raj.front(); // i-raj.front() is added in the waiting time of the approach;
				Raj.pop();
			}
			Raj_ser_index = 0;
		}
		if (Ulu_ser_index >= 1)
		{
			if(!Ulu.empty())
			{
				ulu_wait = ulu_wait + i - Ulu.front();
				Ulu.pop();
			}
			Ulu_ser_index = 0;
		}
		if (ABC_ser_index >= 1)
		{
			if(!ABC.empty())
			{
				abc_wait = abc_wait + i - ABC.front();
				ABC.pop();
			}
			ABC_ser_index = 0;
		}

		Raj_ser_index = Raj_ser_index + Raj_service_rate;
		Ulu_ser_index = Ulu_ser_index + Ulu_service_rate;
		ABC_ser_index = ABC_ser_index + ABC_service_rate;

		if (v_cnt == R_time)
		{
			Raj_service_rate = 0;
			Raj_ser_index = 0;
		}
		if (v_cnt == R_time + 2)
		{
			Ulu_service_rate = 1.0;
		}
		if (v_cnt == R_time + U_time + 2)
		{
			Ulu_ser_index = 0;
			Ulu_service_rate = 0;
		}
		if (v_cnt == R_time + U_time + 4)
		{
			ABC_service_rate = 1.5;
		}
		if (v_cnt == R_time + U_time + A_time + 4)
		{
			ABC_ser_index = 0;
			ABC_service_rate = 0;
		}
		if (v_cnt == R_time + U_time + A_time + 6)
		{
			Raj_service_rate = 0.5;
			v_cnt = 0;
		}
		if (Raj_index <= i)
		{
			Raj.push(i);
			Raj_cnt++;
			Raj_index = Raj_index + Raj_vol;
		}
		if (Ulu_index <= i)
		{
			Ulu.push(i);
			Ulu_cnt++;
			Ulu_index = Ulu_index + Ulu_vol;
		}
		if (ABC_index <= i)
		{
			ABC.push(i);
			ABC_cnt++;
			ABC_index = ABC_index + ABC_vol;
		}
		v_cnt++;
	}
	vector<int> ans;
	ans.push_back(raj_wait);
	ans.push_back(ulu_wait);
	ans.push_back(abc_wait);
	return ans;
}

// Applying genetic algorithm to Find the optimal green times
vector<int> genetic(vector<vector<int>> temp)           // "temp" contains the number of vehicles in each approach;
{
	float mini[3] = { INT_MAX , INT_MAX , INT_MAX };
	vector<vector<int>> v = { {0,0,0},{0,0,0},{0,0,0} };
	// Initializing the three fittest solutions, each having green time between 10 to 60, with interval 10 seconds;
	for (int i = 10; i <= 60; i = i + 10)
	{
		for (int j = 10; j <= 60; j += 10)
		{
			for (int k = 10; k <= 60; k += 10)
			{
				vector<int> w = myfun(i, j, k, temp);
				//Average waiting time for each approach;
				float avg_Raj = 1.0 * w[0] / ((temp[0][0] + temp[0][1] / 5 + temp[0][2])/2);
				float avg_Ulu = 1.0 * w[1] / ((temp[1][0] + temp[1][1] / 5 + temp[1][2])/5);
				float avg_ABC = 1.0 * w[2] / (temp[2][0] + temp[2][1] / 5 + temp[2][2]);
				
				if (mini[2] > abs(avg_Raj - avg_Ulu) + abs(avg_Ulu - avg_ABC) + abs(avg_ABC - avg_Raj))
				{
					v[2][0] = i;
					v[2][1] = j;
					v[2][2] = k;
					mini[2] = abs(avg_Raj - avg_Ulu) + abs(avg_Ulu - avg_ABC) + abs(avg_ABC - avg_Raj);
				}
				else if (mini[1] > abs(avg_Raj - avg_Ulu) + abs(avg_Ulu - avg_ABC) + abs(avg_ABC - avg_Raj))
				{
					v[1][0] = i;
					v[1][1] = j;
					v[1][2] = k;
					mini[1] = abs(avg_Raj - avg_Ulu) + abs(avg_Ulu - avg_ABC) + abs(avg_ABC - avg_Raj);
				}
				else if (mini[0] > abs(avg_Raj - avg_Ulu) + abs(avg_Ulu - avg_ABC) + abs(avg_ABC - avg_Raj))
				{
					v[0][0] = i;
					v[0][1] = j;
					v[0][2] = k;
					mini[0] = abs(avg_Raj - avg_Ulu) + abs(avg_Ulu - avg_ABC) + abs(avg_ABC - avg_Raj);
				}
			}
		}
	}

	// Taking 50th generation solution;
	for (int cnt = 0; cnt < 10; cnt++)
	{
		int Raj_start = min(v[0][0], min(v[1][0], v[2][0]));
		int Ulu_start = min(v[0][1], min(v[1][1], v[2][1]));
		int ABC_start = min(v[0][2], min(v[1][2], v[2][2]));
		int Raj_end = max(v[0][0], max(v[1][0], v[2][0]));
		int Ulu_end = max(v[0][1], max(v[1][1], v[2][1]));
		int ABC_end = max(v[0][2], max(v[1][2], v[2][2]));

		int Raj_gap = max(1,(Raj_end - Raj_start) / 10);
		int Ulu_gap = max(1, (Ulu_end - Ulu_start) / 10);
		int ABC_gap = max(1, (ABC_end - ABC_start) / 10);
		
		// Taking the child solutions of 3 parent solutions with mutation of +-5 the number of child solution depends upon the maximum difference between three best solutions;
		for (int i = Raj_start-5; i <= Raj_end+5; i++)
		{
			for (int j = Ulu_start-5; j <= Ulu_end+5; j++)
			{
				for (int k = ABC_start-5; k <= ABC_end+5; k++)
				{
					vector<int> w = myfun(i, j, k, v);

					float avg_Raj = 1.0 * w[0] / ((temp[0][0] + temp[0][1] / 5 + temp[0][2])/2);
					float avg_Ulu = 1.0 * w[1] / ((temp[1][0] + temp[1][1] / 5 + temp[1][2])/5);
					float avg_ABC = 1.0 * w[2] / (temp[2][0] + temp[2][1] / 5 + temp[2][2]);

					if (mini[2] > abs(avg_Raj - avg_Ulu) + abs(avg_Ulu - avg_ABC) + abs(avg_ABC - avg_Raj))
					{
						v[0][0] = v[1][0];
						v[0][1] = v[1][1];
						v[0][2] = v[1][2];
						v[1][0] = v[2][0];
						v[1][1] = v[2][1];
						v[1][2] = v[2][2];
						v[2][0] = i;
						v[2][1] = j;
						v[2][2] = k;
						mini[2] = abs(avg_Raj - avg_Ulu) + abs(avg_Ulu - avg_ABC) + abs(avg_ABC - avg_Raj);
					}
					else if (mini[1] > abs(avg_Raj - avg_Ulu) + abs(avg_Ulu - avg_ABC) + abs(avg_ABC - avg_Raj))
					{
						v[0][0] = v[1][0];
						v[0][1] = v[1][1];
						v[0][2] = v[1][2];
						v[1][0] = i;
						v[1][1] = j;
						v[1][2] = k;
						mini[1] = abs(avg_Raj - avg_Ulu) + abs(avg_Ulu - avg_ABC) + abs(avg_ABC - avg_Raj);
					}
					else if (mini[0] > abs(avg_Raj - avg_Ulu) + abs(avg_Ulu - avg_ABC) + abs(avg_ABC - avg_Raj))
					{
						v[0][0] = i;
						v[0][1] = j;
						v[0][2] = k;
						mini[0] = abs(avg_Raj - avg_Ulu) + abs(avg_Ulu - avg_ABC) + abs(avg_ABC - avg_Raj);
					}
				}
			}
		}
		
	}
	v[0][0] = max(10, v[0][0]);
	v[0][1] = max(10, v[0][1]);
	v[0][2] = max(10, v[0][2]);
	v[0][0] = min(60, v[0][0]);
	v[0][1] = min(60, v[0][1]);
	v[0][2] = min(60, v[0][2]);
	return v[0];
}


// To convert the date into day 0->Sunday and 6->Saturday
int day(string date)
{
	string d;
	string m;
	string y;
	char ind = 'd';
	for (int i = 0; i < date.size(); i++)
	{
		if (date[i] == '-')
		{
			if (ind == 'd')
				ind = 'm';
			else if (ind == 'm')
				ind = 'y';
		}
		else if (ind == 'd')
		{
			d.push_back(date[i]);
		}
		else if (ind == 'm')
		{
			m.push_back(date[i]);
		}
		else if (ind == 'y')
		{
			y.push_back(date[i]);
		}
	}
	int num = stoi(d);
	int month = stoi(m);
	int flag = -1;
	if (month == 1)
	{
		flag = num % 7;
	}
	else
	{
		flag = (num + 3) % 7;
	}
	return flag;
}

//Seperating the Data according to the Approaches
int seperation()
{
	ifstream in;
	in.open("today.csv");
	if (in.fail())
	{
		cout << "in failed";
		return 1;
	}
	ofstream ulu;
	ofstream raj;
	ofstream abc;
	ulu.open("Ulubari.csv");
	if (ulu.fail())
	{
		cout << "ulu failed";
		return 1;
	}
	raj.open("Rajgarh.csv");
	if (raj.fail())
	{
		cout << "raj failed";
		return 1;
	}
	abc.open("ABC.csv");
	if (abc.fail())
	{
		cout << "abc failed";
		return 1;
	}
	ofstream uluh;
	ofstream rajh;
	ofstream abch;
	uluh.open("Ulubari_holidays.csv");
	if (uluh.fail())
	{
		cout << "uluh failed";
		return 1;
	}
	rajh.open("Rajgarh_holidays.csv");
	if (rajh.fail())
	{
		cout << "rajh failed";
		return 1;
	}
	abch.open("ABC_holidays.csv");
	if (abch.fail())
	{
		cout << "abch failed";
		return 1;
	}
	map<string, int> mpp;
	string s;
	getline(in, s);
	string date;
	string stime;
	string etime;
	string approach;
	string two;
	string three, lmv;
	string hcv, lcv;
	string total;
	abc << "date,day,hour,two wheeler,three wheeler,car,hcv,total\n";
	abch << "date,day,hour,two wheeler,three wheeler,car,hcv,total\n";
	ulu << "date,day,hour,two wheeler,three wheeler,car,hcv,total\n";
	uluh << "date,day,hour,two wheeler,three wheeler,car,hcv,total\n";
	raj << "date,day,hour,two wheeler,three wheeler,car,hcv,total\n";
	rajh << "date,day,hour,two wheeler,three wheeler,car,hcv,total\n";
	while (getline(in, s))
	{
		string cell;
		int cnt = 0;
		stringstream k(s);
		while (getline(k, cell, ','))
		{
			cnt++;
			if (cnt == 2)
			{
				date = cell;
			}
			if (cnt == 3)
			{
				stime = cell;
			}
			if (cnt == 4)
			{
				etime = cell;
			}
			if (cnt == 7)
			{
				approach = cell;
			}
			if (cnt == 8)
			{
				two = cell;
			}
			if (cnt == 9)
				three = cell;
			if (cnt == 10)
			{
				hcv = cell;
			}
			if (cnt == 11)
			{
				lcv = cell;
			}
			if (cnt == 12)
			{
				lmv = cell;
			}
			if (cnt == 15)
			{
				total = cell;
			}
		}
		string start;
		int i = 0;
		while (stime[i] != ':')
		{
			start.push_back(stime[i]);
			i++;
		}
		int hour = stoi(start);
		if (hour == 12)
		{
			if (stime[stime.size() - 2] == 'A')
				hour = 0;
		}
		else if (stime[stime.size() - 2] == 'P')
		{
			hour = hour + 12;
		}
		int h = day(date);
		int car = stoi(lmv) + stoi(lcv);
		if (mpp[date] == 1)
		{
			if (approach == "From Ulubari (Bhangagarh Bridge 1)")
			{
				uluh << date << "," << h << "," << hour << "," << two << "," << three << "," << car << "," << hcv << "," << total << "\n";
			}
			if (approach == "From ABC Point (Bhangagarh Bridge 1)")
			{
				abch << date << "," << h << "," << hour << "," << two << "," << three << "," << car << "," << hcv << "," << total << "\n";
			}
			if (approach == "From Rajgarh (Bhangagarh Bridge 1)")
			{
				rajh << date << "," << h << "," << hour << "," << two << "," << three << "," << car << "," << hcv << "," << total << "\n";
			}
		}
		else
		{
			if (approach == "From Ulubari (Bhangagarh Bridge 1)")
			{
				ulu << date << "," << h << "," << hour << "," << two << "," << three << "," << car << "," << hcv << "," << total << "\n";
			}
			if (approach == "From ABC Point (Bhangagarh Bridge 1)")
			{
				abc << date << "," << h << "," << hour << "," << two << "," << three << "," << car << "," << hcv << "," << total << "\n";
			}
			if (approach == "From Rajgarh (Bhangagarh Bridge 1)")
			{
				raj << date << "," << h << "," << hour << "," << two << "," << three << "," << car << "," << hcv << "," << total << "\n";
			}
		}
	}
	abc.close();
	abch.close();
	raj.close();
	rajh.close();
	ulu.close();
	uluh.close();
	in.close();
}

// Taking data from file today.csv and adding it to the relevent places
void update(string filename, vector<string> &v)
{
	ifstream f;
	f.open(filename);
	if (f.fail())
	{
		cout << "update file failed to open !!";
		return;
	}
	string s;
	getline(f, s);
	string date;
	string time;
	string approach;
	string two,car,hcv;
	int car1, car2;
	int hour_cnt=0;
	int ex_hr=0;

	while (getline(f, s))
	{
		stringstream ss(s);
		string cell;
		int cnt = 0;
		while (getline(ss, cell, ','))
		{
			cnt++;
			if (cnt == 1)
				date = cell;
			if (cnt == 3)
				time = cell;
			if (cnt == 4)
				two = cell;
			if (cnt == 7)
				hcv = cell;
			if (cnt == 6)
				car = cell;
		}

		if (ex_hr == stoi(time))
		{
			hour_cnt++;
			if (hour_cnt == 6)
			{
				hour_cnt = 0;
				ex_hr++;
			}
			string ans = date + "," + time + "," + two + "," + car + "," + hcv;
			v.push_back(ans);
		}
		else
		{
			for (int i = hour_cnt; i < 6; i++)
			{
				string ans = date + "," + to_string(ex_hr) + "," + two + "," + car + "," + hcv;
				v.push_back(ans);
			}
			string ans = date + "," + time + "," + two + "," + car + "," + hcv;
			hour_cnt = 1;
			ex_hr++;
		}
	}
}

//Main function
int main()
{
	for (int i = 10; i < 23; i++)
	{
		vector<vector<int>> v = predict(to_string(i));
		vector<int> ans = genetic(v);
		cout << "The distribution of green-time between i to i+1";
		cout <<"Rajgarh Approach"<< ans[0] << endl;
		cout << "Ulubari Approach"<< ans[1] << endl;
		cout << "ABC Point Approach"<< ans[2] << endl;
		vector<string> vs=read("Raj_inputs.csv", to_string(i));
		write("Raj_inputs.csv", vs);
		vs=read("ABC_inputs.csv", to_string(i + 1));
		write("ABC_inputs.csv", vs);
		vs=read("Ulu_inputs.csv", to_string(i + 1));
		write("Ulu_inputs.csv", vs);
	}
	seperation();
	vector<vector<int>> v = predict("23");
	vector<int> ans = genetic(v);

	cout << ans[0] << endl;
	cout << ans[1] << endl;
	cout << ans[2] << endl;

	vector<string> Raj=read("Raj_inputs.csv", "23");
	vector<string> ABC = read("ABC_inputs.csv", "23");
	vector<string> Ulu=read("Ulu_inputs.csv", "23");

	update("Rajgarh.csv", Raj);
	update("Ulubari.csv", Ulu);
	update("ABC.csv", ABC);

	write("Raj_inputs.csv", Raj);
	write("ABC_inputs.csv", ABC);
	write("Ulu_inputs.csv", Ulu);
}
