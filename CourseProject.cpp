#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <limits>
#include <ctime>
using namespace std;

// Define a structure to hold course information
struct Course {
    string courseNumber; // Unique identifier
    string title;
    vector<string> prerequisites; // List of prerequisite course numbers
    Course() {}
};

// Internal structure for tree node
struct Node {
    Course course;
    Node* left;
    Node* right;
    // Default constructor
    Node() {
        left = nullptr;
        right = nullptr;
    }
    // Initialize with a course
    Node(Course aCourse) : Node() {
        course = aCourse;
    }
};

// Binary Search Tree class definition
class BinarySearchTree {
private:
    Node* root;
    void addNode(Node* node, Course course);
    void inOrder(Node* node);
    Node* findMin(Node* node); // Helper for removal (not used but kept for completeness)
    Node* removeNode(Node* node, string courseNumber);

public:
    BinarySearchTree();
    ~BinarySearchTree();
    void InOrder();
    void Insert(Course course);
    Course Search(string courseNumber);
};

// Default constructor
BinarySearchTree::BinarySearchTree() {
    root = nullptr; // Initialize root to nullptr
}

// Destructor
BinarySearchTree::~BinarySearchTree() {
    while (root != nullptr) {
        root = removeNode(root, root->course.courseNumber); // Remove nodes recursively
    }
}

// Traverse the tree in order (alphanumeric by course number)
void BinarySearchTree::InOrder() {
    inOrder(root); // Call recursive in-order traversal
}

// Insert a course
void BinarySearchTree::Insert(Course course) {
    if (root == nullptr) {
        root = new Node(course); // Create new root if tree is empty
    }
    else {
        addNode(root, course); // Recursively add to appropriate position
    }
}

// Search for a course
Course BinarySearchTree::Search(string courseNumber) {
    Node* current = root;
    while (current != nullptr) {
        if (current->course.courseNumber == courseNumber) {
            return current->course; // Return matching course
        }
        if (courseNumber < current->course.courseNumber) {
            current = current->left;
        }
        else {
            current = current->right;
        }
    }
    return Course(); // Return empty course if not found
}

// Add a course to some node (recursive)
void BinarySearchTree::addNode(Node* node, Course course) {
    if (course.courseNumber < node->course.courseNumber) {
        if (node->left == nullptr) {
            node->left = new Node(course); // Insert as left child
        }
        else {
            addNode(node->left, course); // Recurse down left subtree
        }
    }
    else {
        if (node->right == nullptr) {
            node->right = new Node(course); // Insert as right child
        }
        else {
            addNode(node->right, course); // Recurse down right subtree
        }
    }
}

// In-order traversal (Left, Root, Right)
void BinarySearchTree::inOrder(Node* node) {
    if (node != nullptr) {
        inOrder(node->left); // Traverse left subtree
        // Display course information
        cout << node->course.courseNumber << ": " << node->course.title << endl;
        inOrder(node->right); // Traverse right subtree
    }
}

// Find the minimum node in a subtree
Node* BinarySearchTree::findMin(Node* node) {
    Node* current = node;
    while (current && current->left != nullptr) {
        current = current->left;
    }
    return current;
}

// Remove a course from some node (recursive)
Node* BinarySearchTree::removeNode(Node* node, string courseNumber) {
    if (node == nullptr) {
        return node;
    }
    if (courseNumber < node->course.courseNumber) {
        node->left = removeNode(node->left, courseNumber);
    }
    else if (courseNumber > node->course.courseNumber) {
        node->right = removeNode(node->right, courseNumber);
    }
    else {
        if (node->left == nullptr && node->right == nullptr) {
            delete node;
            return nullptr;
        }
        else if (node->left == nullptr) {
            Node* temp = node->right;
            delete node;
            return temp;
        }
        else if (node->right == nullptr) {
            Node* temp = node->left;
            delete node;
            return temp;
        }
        else {
            Node* temp = findMin(node->right);
            node->course = temp->course;
            node->right = removeNode(node->right, temp->course.courseNumber);
        }
    }
    return node;
}

// Display course information to the console
void displayCourse(const Course& course, BinarySearchTree* bst) {
    if (course.courseNumber.empty()) {
        cout << "Course not found." << endl;
        return;
    }
    cout << course.courseNumber << ": " << course.title << endl;
    cout << "Prerequisites: ";
    if (course.prerequisites.empty()) {
        cout << "None" << endl;
    }
    else {
        for (size_t i = 0; i < course.prerequisites.size(); ++i) {
            Course prereq = bst->Search(course.prerequisites[i]);
            if (!prereq.courseNumber.empty()) {
                cout << prereq.courseNumber << " (" << prereq.title << ")";
            }
            else {
                cout << course.prerequisites[i] << " (Title not found)";
            }
            if (i < course.prerequisites.size() - 1) cout << ", ";
        }
        cout << endl;
    }
}

// Load course data from a file into the BST
void loadCourses(const string& filePath, BinarySearchTree* bst) {
    cout << "Loading CSV file " << filePath << endl;
    ifstream file(filePath);
    if (!file.is_open()) {
        cout << "Error: Unable to open file " << filePath << endl;
        return;
    }
    string line;
    unsigned int courseCount = 0;
    while (getline(file, line)) {
        if (line.empty()) continue; // Skip empty lines
        stringstream ss(line);
        Course course;
        string token;
        // Read course number
        getline(ss, course.courseNumber, ',');
        // Read course title
        getline(ss, course.title, ',');
        // Read prerequisites (if any)
        while (getline(ss, token, ',')) {
            if (!token.empty() && token != "None") {
                course.prerequisites.push_back(token);
            }
        }
        bst->Insert(course);
        courseCount++;
    }
    file.close();
    cout << courseCount << " courses read" << endl;
}

// Display the menu
void displayMenu() {
    cout << "Menu:" << endl;
    cout << "  1. Load Course Data" << endl;
    cout << "  2. Display All Courses" << endl;
    cout << "  3. Find Course" << endl;
    cout << "  9. Exit" << endl;
    cout << "Enter choice: ";
}

int main() {
    BinarySearchTree* bst = new BinarySearchTree();
    string filePath;
    const string defaultFilePath = "CS 300 ABCU_Advising_Program_Input.csv"; // Updated default file path
    int choice = 0;
    bool dataLoaded = false;
    clock_t ticks; // For timing operations

    while (choice != 9) {
        displayMenu();
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer

        if (cin.fail() || (choice != 1 && choice != 2 && choice != 3 && choice != 9)) {
            cout << "Invalid choice. Please enter 1, 2, 3, or 9." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
        case 1: {
            ticks = clock(); // Start timing
            char useDefault;
            cout << "Use default file path (" << defaultFilePath << ")? (y/n): ";
            cin >> useDefault;
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer
            useDefault = tolower(useDefault); // Case-insensitive input
            if (useDefault == 'y') {
                filePath = defaultFilePath;
            }
            else if (useDefault == 'n') {
                cout << "Enter the course data file path: ";
                getline(cin, filePath);
            }
            else {
                cout << "Invalid input. Please enter 'y' or 'n'." << endl;
                ticks = clock() - ticks; // End timing
                cout << "time: " << ticks << " clock ticks" << endl;
                cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;
                break;
            }
            loadCourses(filePath, bst);
            dataLoaded = true;
            ticks = clock() - ticks; // End timing
            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;
            break;
        }
        case 2: {
            ticks = clock(); // Start timing
            if (!dataLoaded) {
                cout << "Error: No course data loaded. Please load data first." << endl;
            }
            else {
                cout << "List of Courses (Alphanumeric Order):" << endl;
                bst->InOrder();
            }
            ticks = clock() - ticks; // End timing
            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;
            break;
        }
        case 3: {
            ticks = clock(); // Start timing
            if (!dataLoaded) {
                cout << "Error: No course data loaded. Please load data first." << endl;
            }
            else {
                string courseNumber;
                cout << "Enter course number: ";
                getline(cin, courseNumber);
                Course course = bst->Search(courseNumber);
                displayCourse(course, bst);
            }
            ticks = clock() - ticks; // End timing
            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;
            break;
        }
        case 9: {
            cout << "Good bye." << endl;
            break;
        }
        }
    }

    delete bst; // Free the tree
    return 0;
}