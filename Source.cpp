
#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>
#include <utility>
#include <type_traits>

using namespace std;

struct Student {
	std::string fname;
	std::string lname;
	std::string name;
	std::string uid;
	std::string email;
	double grade_presentation, grade_essay, grade_term_project;

	Student(std::string _fname, std::string _lname, std::string _uid, std::string _email,
		double g_presentation, double g_essay, double g_term_project)
		: fname(_fname), lname(_lname), name(_fname + " " + _lname), email(_email), uid(_uid),
		grade_presentation(g_presentation), grade_essay(g_essay),
		grade_term_project(g_term_project) {}

	friend std::ostream& operator<<(std::ostream& os, const Student& s)
	{
		os << s.name << ' ' << s.uid << ' '
			<< s.email << ' ' << s.grade_presentation << ' '
			<< s.grade_essay << ' ' << s.grade_term_project;
		return os;
	}
};

struct Classroom {
	std::string filename;
	enum searchTypes { UID, EMAIL, NAME };
	std::vector<Student> students;

	Classroom(const std::string& _filename) : filename(_filename) {
		std::ifstream file_reader(filename);
		std::string line;
		while (std::getline(file_reader, line)) {
			std::string fname;
			std::string lname;
			std::string uid;
			std::string email;
			double grade_presentation, grade_essay, grade_term_project;

			std::istringstream iss(line);
			iss >> fname >> lname >> uid >> email >> grade_presentation >> grade_essay >> grade_term_project;
			students.emplace_back(fname, lname, uid, email, grade_presentation, grade_essay, grade_term_project);
		}
	}

	~Classroom() {
		std::ofstream file_writer(filename, std::ios::trunc | std::ios::out);
		for (const auto& student : students) {
			file_writer << student << '\n';
		}
	}

	bool addStudent(const Student& s)
	{
		// check if student already exists
		if (searchStudent(s.uid, UID).second) {
			std::cout << "Error: Could not insert. Student already exists!\n";
			return false;
		}
		else {
			students.push_back(s);
			return true;
		}
	}
	bool deleteStudent(const Student& s) {
		auto p = searchStudent(s.uid, UID);
		if (!p.second) {
			std::cout << "Error: Student with that USF ID does not exist!\n";
			return false;
		}
		else {
			students.erase(p.first);
			return true;
		}
	}
	std::pair<std::vector<Student>::iterator, bool> searchStudent(const std::string& s, const searchTypes& t) {
		auto search = std::find_if(students.begin(), students.end(),
			[&](const Student& element) {
			if (t == UID)
				return element.uid == s;
			else if (t == EMAIL)
				return element.email == s;
			else
				return element.name == s;
		}
		);
		return { search, search != students.end() };
	}
};

struct Menu {
	Classroom classroom;

	Menu(const string& _filename) : classroom(Classroom(_filename))
	{
		getUserInput();
	}

	void getUserInput()
	{
		char option{};
		while (option != 'q')
		{
			displayMenuOptions();
			cin >> option;
			selectOption(option);
		}
	}

	void displayMenuOptions()
	{
		std::cout << "\n"
			<< "Press one of the following keys: \n"
			<< "a: add student\n"
			<< "d: delete student\n"
			<< "s: search student\n"
			<< "u: update data fields\n"
			<< "q: quit\n"
			<< "Enter option: ";
	}

	void selectOption(const char option)
	{
		switch (option)
		{
		case 'a':
			addStudentCLI();
			break;
		case 'd':
			deleteStudentCLI();
			break;
		case 's':
			searchStudentCLI();
			break;
		case 'u':
			updateStudentCLI();
			break;
		default:
			break;
		}
	}

	void clearBuffer()
	{
		cin.clear();
		cin.ignore(numeric_limits<int>::max(), '\n');
	}

	void validateInputStrings_addStudentCLI(string& userInput, const string& inputRequest, const size_t& s)
	{
		getline(cin, userInput);

		// check size
		while (userInput.length() > s)
		{
			cout << "Error: " << inputRequest << " is limited to " << s << " characters.\n";
			cout << "Enter " << inputRequest << ": ";
			getline(cin, userInput);
		}

		// ensure no spaces
		while (userInput.find(' ') <= s)
		{
			cout << "Error: " << inputRequest << " must not contain spaces.\n";
			cout << "Enter " << inputRequest << ": ";
			getline(cin, userInput);
		}
	}
	void validateInputDoubles_addStudentCLI(double& grade, const string& assignment)
	{
		while (!(cin >> grade) || grade < 0 || grade > 4)
		{
			clearBuffer();
			cout << "Error: " << assignment << " grade must be a double from 0 to 4.\n";
			cout << "Enter " << assignment << " grade: ";
		}
	}
	void addStudentCLI()
	{
		// create member variables for new student
		std::string fname{};
		std::string lname{};
		std::string uid{};
		std::string email{};
		double grade_presentation, grade_essay, grade_term_project;

		// accept user input
		clearBuffer();
		cout << "Enter first name: ";
		validateInputStrings_addStudentCLI(fname, "first name", 40);
		cout << "Enter last name: ";
		validateInputStrings_addStudentCLI(lname, "last name", 40);
		cout << "Enter UID: ";
		validateInputStrings_addStudentCLI(uid, "USF ID", 10);
		cout << "Enter email: ";
		validateInputStrings_addStudentCLI(email, "email", 40);
		cout << "Enter presentation grade: ";
		validateInputDoubles_addStudentCLI(grade_presentation, "presentation");
		cout << "Enter essay grade: ";
		validateInputDoubles_addStudentCLI(grade_essay, "essay");
		cout << "Enter term project grade: ";
		validateInputDoubles_addStudentCLI(grade_term_project, "essay");

		// add student to classroom
		if (classroom.addStudent(Student(fname, lname, uid, email, grade_presentation, grade_essay, grade_term_project)))
		{
			cout << "Student added successfully.\n";
		}
	}

	void deleteStudentCLI()
	{
		// accept user input of UID
		string uid;
		cout << "Enter UID of student to delete: ";
		cin >> uid;

		// delete student
		auto studentPair = classroom.searchStudent(uid, classroom.UID);
		if (!studentPair.second)
		{
			cout << "Error: Student with that USF ID does not exist!\n";
		}
		else if (classroom.deleteStudent(*studentPair.first))
		{
			cout << "Student deleted successfully.\n";
		}
	}

	std::pair<std::vector<Student>::iterator, bool> searchStudentCLI()
	{
		// ask user to select the search type
		int option;
		cout << "Enter 'n' to search by name, 'i' to search by ID, or 'e' to search by email: ";
		clearBuffer();
		option = getchar();

		string searchQuery;
		bool validOptionChosen = true;
		std::pair<std::vector<Student>::iterator, bool> searchResult;
		// perform search based on search type
		switch (option)
		{
		case 'n':
			cout << "Enter name: ";
			clearBuffer();
			getline(cin, searchQuery);
			searchResult = classroom.searchStudent(searchQuery, classroom.NAME);
			break;
		case 'i':
			cout << "Enter ID: ";
			cin >> searchQuery;
			searchResult = classroom.searchStudent(searchQuery, classroom.UID);
			break;
		case 'e':
			cout << "Enter email: ";
			cin >> searchQuery;
			searchResult = classroom.searchStudent(searchQuery, classroom.EMAIL);
			break;
		default:
			clearBuffer();
			cout << "Error: Invalid selection.\n";
			validOptionChosen = false;
			break;
		}

		if (validOptionChosen)
		{
			// output search result
			if (searchResult.second)
			{
				cout << "Name, USF ID, Email, Presentation grade, Essay grade, Term Project grade\n";
				cout << *searchResult.first << endl;
				return searchResult;
			}
			else
			{
				cout << "Error: No results found\n";
				return searchResult;
			}
		}
	}

	void updateStudentCLI()
	{
		std::pair<std::vector<Student>::iterator, bool> searchResult = searchStudentCLI();
		if (searchResult.second)
		{
			// get user input for field to update
			int field;
			cout << "Which field would you like to update? Select one of the following options\n"
				<< "n: name\n"
				<< "u: USF ID\n"
				<< "m: email\n"
				<< "p: presentation grade\n"
				<< "e: essay grade\n"
				<< "t: term project grade\n"
				<< "Enter option: ";
			field = getchar();

			switch (field)
			{
			case 'n':

			default:
				break;
			}
		}
	}
};

int main(int argc, char *argv[])
{
	//Classroom c("E:\\ST_Classroom.txt");
	//c.addStudent(Student("Gisela", "Palmer", "31788XXXX", "GiselaKPalmer@dayrep.com", 0, 1, 2));
	//c.addStudent(Student("Jeffrey", "Chafin", "23125XXXX", "JeffreyCChafin@rhyta.com", 0, 1, 2));
	//c.addStudent(Student("William", "Ramirez", "61313XXXX", "WilliamGRamirez@jourrapide.com", 0, 1, 2));
	//c.addStudent(Student("Paul", "Snyder", "46702XXXX", "PaulMSnyder@jourrapide.com", 0, 1, 2));
	//c.addStudent(Student("Melissa", "McDonald", "76507XXXX", "MelissaMMcDonald@teleworm.us", 0, 1, 2));
	//c.addStudent(Student("Kia", "Johnson", "57929XXXX", "KiaMJohnson@dayrep.com", 0, 1, 2));
	//std::cout<<*c.searchStudent("2312XXXX", c.UID).first;

	//std::cout << "Enter file name: ";
	std::string filename;
	//std::cin >> filename;
	//Menu menu(filename);
	Menu menu("E:\\ST_Classroom.txt");
	return 0;
}







//
//int main(int argc, char *argv[])
//{
//	Classroom c("E:\\ST_Classroom.txt");
//	c.deleteStudent(c.searchStudent("317-88-XXXX",c.UID));
//	c.addStudent(Student("Jeffrey", "Chafin", "231-25-XXXX", "JeffreyCChafin@rhyta.com", 0, 1, 2));
//	c.addStudent(Student("William", "Ramirez", "613-13-XXXX", "WilliamGRamirez@jourrapide.com", 0, 1, 2));
//	c.addStudent(Student("Paul", "Snyder", "467-02-XXXX", "PaulMSnyder@jourrapide.com", 0, 1, 2));
//	c.addStudent(Student("Melissa", "McDonald", "765-07-XXXX", "MelissaMMcDonald@teleworm.us", 0, 1, 2));
//	c.addStudent(Student("Kia", "Johnson", "579-29-XXXX", "KiaMJohnson@dayrep.com", 0, 1, 2));
//	//c.addStudent(Student("Aurora", "Avila", "551-14-XXXX", "AuroraLAvila@jourrapide.com", 0, 1, 2));
//}