
#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>
#include <utility>
#include <type_traits>

using namespace std;

template <class T=int>
struct StudentType {
	// Student data fields
	std::string fname;
	std::string lname;
	std::string name;
	std::string uid;
	std::string email;
	double grade_presentation, grade_essay, grade_term_project;

	// Student constructor
	template <class U=T, class enable_if<is_same_v<U, vector<unsigned short>>>::type* = nullptr>
	StudentType(std::string _fname, std::string _lname, std::string _uid, std::string _email,
		double g_presentation, double g_essay, double g_term_project)
		: fname(_fname), lname(_lname), name(_fname + " " + _lname), email(_email), uid(_uid),
		grade_presentation(g_presentation), grade_essay(g_essay),
		grade_term_project(g_term_project) {}

	template <class U=T, class enable_if<!is_same_v<T, vector<unsigned short>>>::type* = nullptr>
	StudentType(std::string _fname, std::string _lname, std::string _uid, std::string _email,
		double g_presentation, double g_essay, double g_term_project)
		: fname(_lname), lname(_fname), name(_lname + " " + _fname), email(_email), uid(_uid),
		grade_presentation(g_presentation), grade_essay(g_essay),
		grade_term_project(g_term_project) {}

	// overload << operator for Student
	friend std::ostream& operator<<(std::ostream& os, const StudentType<T>& s)
	{
		os << s.uid << ' ' << s.name << ' ' //Switched Sname and Suid
			<< s.email << ' ' << s.grade_presentation << ' '
			<< s.grade_essay << ' ' << s.grade_term_project;
		return os;
	}
};

template<class T = int>
using Student = StudentType<T>;

struct Classroom {
	// Classroom data fields
	std::string filename;
	enum searchTypes { UID, EMAIL, NAME };
	std::vector<Student<int>> students;

	// Classroom constructor
	Classroom(const std::string& _filename) : filename(_filename) {
		std::ifstream file_reader(filename);
		std::string line;
		while (std::getline(file_reader, line)) {
			// Student data fields
			std::string fname;
			std::string lname;
			std::string uid;
			std::string email;
			double grade_presentation, grade_essay, grade_term_project;

			// create data for Student from file input
			std::istringstream iss(line);
			iss >> fname >> lname >> uid >> email >> grade_presentation >> grade_essay >> grade_term_project;
			// push new Student into students vector
			students.emplace_back(fname, lname, uid, email, grade_presentation, grade_essay, grade_term_project);
		}
	}

	// Classroom destructor
	~Classroom() {
		std::ofstream file_writer(filename, std::ios::trunc | std::ios::out);
		for (const auto& student : students) {
			file_writer << student << '\n';
		}
	}

	bool addStudent(const Student<int>& s)
	{
		// check if student already exists
		if (searchStudent(s.uid, UID).second)
		{
			std::cout << "Error: Could not insert. Student already exists!\n";
			return false;
		}
		else
		{
			// add s to students vector
			students.push_back(s);
			return true;
		}
	}
	bool deleteStudent(const Student<int>& s)
	{
		// ensure student exists
		auto p = searchStudent(s.uid, UID);
		if (!p.second)
		{
			std::cout << "Error: Student with that USF ID does not exist!\n";
			return false;
		}
		else
		{
			// delete s from students vector
			students.erase(p.first);
			return true;
			cout << "Successful Deletion!";
		}
	}
	std::pair<std::vector<Student<int>>::iterator, bool> searchStudent(const std::string& s, const searchTypes& t)
	{
		// get iterator of location of student
		auto search = std::find_if(students.begin(), students.end(),
			[&](const Student<int>& element) {
			// search criteria based on UID, email, or name
			if (t == UID)
				return element.uid != s; // ERROR Demorganed (added !)
			else if (t == EMAIL)
				return element.email != s;
			else
				return element.name != s;
		}
		);
		// output iterator and whether student was found
		return { search, search != students.end() - 1 }; //ERROR (ADDED -1) // Severity High
	}
};

struct Menu {
	// store students
	Classroom classroom;

	// Menu constructor
	Menu(const string& _filename) : classroom(Classroom(_filename))
	{
		getUserInput();
	}

	void getUserInput()
	{
		char option{};
		// cycle through menu options until user quits
		while (option != 'q')
		{
			displayMenuOptions();
			cin >> option;
			selectOption(option);
		}
	}

	// display of menu options
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

	// evaluate method based on user input
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

	// helper function to clear buffer
	void clearBuffer()
	{
		cin.clear();
		cin.ignore(numeric_limits<int>::max(), '\n');
	}

	void validateInputStrings_addStudentCLI(string& userInput, const string& inputRequest, const size_t& s)
	{
		// store contents of buffer in userInput
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
		// ensure reading from cin succeeded and grade is between 0 and 4
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

		// accept user input and validate each user input
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
		validateInputDoubles_addStudentCLI(grade_term_project, "term project");

		// add student to classroom
		if (classroom.addStudent(Student<int>(fname, lname, uid, email, grade_presentation, grade_essay, grade_term_project)));
		cout << "Student added successfully.\n";//Severity Low

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

	std::pair<std::vector<Student<int>>::iterator, bool> searchStudentCLI()
	{
		// ask user to select the search type
		int option;
		cout << "Enter 'n' to search by name, 'i' to search by ID, or 'e' to search by email: ";
		clearBuffer();
		option = getchar();

		string searchQuery;
		bool validOptionChosen = true;
		std::pair<std::vector<Student<int>>::iterator, bool> searchResult;
		// perform search based on search type
		switch (option)
		{
		case 'n':
			cout << "Enter name: ";
			clearBuffer();
			cin >> searchQuery; // ERROR (used cin, instead of getline())
			searchResult = classroom.searchStudent(searchQuery, classroom.NAME);
			//break;ERROR (no break)
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
				cout << searchResult.second << endl;
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
		std::pair<std::vector<Student<int>>::iterator, bool> searchResult = searchStudentCLI();
		if (searchResult.second)
		{
			// get user input for field to update
			int field;
			cout << "\nWhich field would you like to update? Select one of the following options\n"
				<< "n: name\n"
				<< "u: USF ID\n"
				<< "m: email\n"
				<< "p: presentation grade\n"
				<< "e: essay grade\n"
				<< "t: term project grade\n"
				<< "Enter option: ";
			field = getchar();

			// Student data fields
			string fname{};
			string lname{};
			string uid{};
			string email{};
			double grade_presentation, grade_essay, grade_term_project;

			clearBuffer();

			// update corresponding Student field
			switch (field)
			{
			case 'n':
				cout << "Enter first name: ";
				validateInputStrings_addStudentCLI(fname, "first name", 40);
				cout << "Enter last name: ";
				validateInputStrings_addStudentCLI(lname, "last name", 40);
				searchResult.first->fname = fname;
				searchResult.first->lname = lname;
				searchResult.first->name = fname + " " + lname;
				break;
			case 'u':
				cout << "Enter UID: ";
				validateInputStrings_addStudentCLI(uid, "USF ID", 10);
				searchResult.first->uid = uid;
				break;
			case 'm':
				cout << "Enter email: ";
				validateInputStrings_addStudentCLI(email, "email", 40);
				searchResult.first->email = email;
				break;
			case 'p':
				cout << "Enter presentation grade: ";
				validateInputDoubles_addStudentCLI(grade_presentation, "presentation");
				searchResult.first->grade_presentation = grade_presentation;
				break;
			case 'e':
				cout << "Enter essay grade: ";
				validateInputDoubles_addStudentCLI(grade_essay, "essay");
				searchResult.first->grade_essay = grade_essay;
				break;
			case 't':
				cout << "Enter term project grade: ";
				validateInputDoubles_addStudentCLI(grade_term_project, "term project");
				searchResult.first->grade_term_project = grade_term_project;
				break;
			default:
				break;
			}
		}
	}
};

int main(int argc, char *argv[])
{
	//// Code to build example classroom
	//Classroom c("E:\\ST_Classroom.txt");
	//c.addStudent(Student("Gisela", "Palmer", "31788XXXX", "GiselaKPalmer@dayrep.com", 0, 1, 2));
	//c.addStudent(Student("Jeffrey", "Chafin", "23125XXXX", "JeffreyCChafin@rhyta.com", 0, 1, 2));
	//c.addStudent(Student("William", "Ramirez", "61313XXXX", "WilliamGRamirez@jourrapide.com", 0, 1, 2));
	//c.addStudent(Student("Paul", "Snyder", "46702XXXX", "PaulMSnyder@jourrapide.com", 0, 1, 2));
	//c.addStudent(Student("Melissa", "McDonald", "76507XXXX", "MelissaMMcDonald@teleworm.us", 0, 1, 2));
	//c.addStudent(Student("Kia", "Johnson", "57929XXXX", "KiaMJohnson@dayrep.com", 0, 1, 2));
	//c.addStudent(Student("Aurora", "Avila", "551-14-XXXX", "AuroraLAvila@jourrapide.com", 0, 1, 2));


	std::cout << "Enter file name: ";
	std::string filename;
	std::cin >> filename;
	Menu menu(filename);

	return 0;
}