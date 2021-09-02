#include <cmath>
#include <limits>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>

#define NaN std::numeric_limits<double>::quiet_NaN() //Used to mark unitialized double values
#define EPSILON std::numeric_limits<double>::epsilon() //Used to reliably compare double values

//Returns -1 if value1 is smaller, 1 if value 2 is smaller, 0 if equals
int compareDoubles(double value1, double value2) { 
	if(value1 - value2 < -EPSILON)
		return -1;
	else if(value1 - value2 > EPSILON)
		return 1;
	else
		return 0;
}

class Point2D {
public:
	double x, y;
	
	//Simple constructor
	Point2D(double x, double y) : x(x), y(y) {}

	//Returns the nth dimension value of the point
	double get(int dimension) {
		return dimension ? y : x;
	}
	
	//Compares the nth dimension of 2 points, return values are same as the compareDoubles()
	int compareDimension(Point2D* point, int dimension) {
		return compareDoubles(get(dimension), point->get(dimension));
	}

	//Returns if two points are equal or not
	bool equals(Point2D* point) {
		return !compareDimension(point, 0) && !compareDimension(point, 1);
	}

	//Returns if this point is inside the given rectangular area defined by 2 external points
	bool inside(Point2D* lowerLeft, Point2D* upperRight) { 
		return compareDimension(lowerLeft, 0) > -1 && compareDimension(upperRight, 0) < 1 &&
				compareDimension(lowerLeft, 1) > -1 && compareDimension(upperRight, 1) < 1;
	}
};

//Friend function to print points easier, simply ignore
std::ostream& operator<<(std::ostream& stream, Point2D* point) {
	return stream << "(" << point->x << "," << point->y <<")";
}

class KDTree { //This class acts as a tree node. Tree is simply a reference to the root node
public:
	int dimension; //Which dimension does this layer holds, -1 if it is a leaf node, -2 if root and empty
	double median; //Value that is used to split the given axis
	Point2D* point; //Reference to the point, null if not a leaf

	KDTree* parent;
	KDTree* left;
	KDTree* right;

	//Simple node constructor
	KDTree(int dimension, double median, Point2D* point, KDTree* parent) :
		dimension(dimension), median(median), point(point), parent(parent), left(nullptr), right(nullptr) {}

	//Destructor to prevent memory leaks, simply ignore this and all other "delete" lines
	~KDTree() {
		delete point;
		delete left;
		delete right;
	}

	//Inserts the given point into the KDTree
	void insert(Point2D* newPoint) {
		if(dimension == -2) { //If it is root and empty
			dimension = -1;
			point = newPoint;
		} else if(dimension == -1) { //If we reached the leaf
			dimension = parent ? (parent->dimension + 1) % 2 : 0;
			median = std::min<double>(newPoint->get(dimension), point->get(dimension));

			if(point->compareDimension(newPoint, dimension) < 0) {
				left = new KDTree(-1, NaN, point, this);
				right = new KDTree(-1, NaN, newPoint, this);
			} else {
				left = new KDTree(-1, NaN, newPoint, this);
				right = new KDTree(-1, NaN, point, this);
			}

			point = nullptr;
		} else if (dimension > -1) { //If it is not a leaf, choose which side we recursively dive
			if(compareDoubles(newPoint->get(dimension), median) < 1)
				left->insert(newPoint);
			else
				right->insert(newPoint);
		}
	}

	//Function to remove the node that holds the given point from the tree
	void remove(Point2D* oldPoint) {
		if(dimension == -1) {
			if(point->equals(oldPoint)) {
				delete point;
				point = nullptr;

				if(parent) { //If it is not the root node
					KDTree* sibling = this == parent->left ? parent->right : parent->left;

					parent->dimension = -1;
					parent->median = NaN;
					parent->point = sibling->point;
					parent->left = nullptr;
					parent->right = nullptr;

					sibling->point = nullptr;
					delete sibling;
					delete this;
				} else
					dimension = -2;
			}
		} else if(dimension == -2) //If the tree is empty
			return;
		else if(compareDoubles(oldPoint->get(dimension), median) < 1)
			left->remove(oldPoint);
		else
			right->remove(oldPoint);
	}

	//Search the given point in the tree
	Point2D* search(Point2D* targetPoint) {
		if(dimension == -1 && point->equals(targetPoint)) //Found
			return point;
		else if(dimension < 0) //Reached a leaf and not found
			return nullptr;
		else if(compareDoubles(targetPoint->get(dimension), median) < 1)
			return left->search(targetPoint);
		else
			return right->search(targetPoint);
	}

	//Find the point with with smallest nth dimension
	Point2D* findMin(int targetDimension) {
		if(dimension < 0) //Found the min
			return point;
		else if(dimension == targetDimension) //Search one branch depending on the values if same dimension
			return left->findMin(targetDimension);
		else { //Need to search both branches if another dimension
			Point2D* leftMin = left->findMin(targetDimension);
			Point2D* rightMin = right->findMin(targetDimension);
			//Return the smaller of two
			return leftMin->compareDimension(rightMin, targetDimension) < 1 ? leftMin : rightMin;
		}
	}

	//Find the point with with largest nth dimension
	Point2D* findMax(int targetDimension) {
		if(dimension < 0) //Found the max
			return point;
		else if(dimension == targetDimension) //Search one branch depending on the values if same dimension
			return right->findMax(targetDimension);
		else { //Need to search both branches if another dimension
			Point2D* leftMax = left->findMax(targetDimension);
			Point2D* rightMax = right->findMax(targetDimension);
			//Return the larger of two
			return leftMax->compareDimension(rightMax, targetDimension) < 1 ? rightMax : leftMax;
		}
	}

	//Display leaves from left to right, line by line
	void displayPoints() {
		if(dimension > -1) {
			left->displayPoints();
			right->displayPoints();
		} else if(dimension == -1)
			std::cout << point << std::endl;
	}

	//Display the tree in a human readable format
	void displayTree(int level = 0) { //0 is the default parameter value, used if the function is called without parameter
		for(int i = 0; i < level; i++) //Print dots depending on layer
			std::cout << ". ";

		if (dimension > -1) { //If not a leaf, print axis info
			std::cout << (dimension ? "(-: (y=" : "(|: (x=") << median << "))" << std::endl;
			//Preorder traversal
			left->displayTree(level + 1);
			right->displayTree(level + 1);
		} else if(dimension == -1) //If leaf, print point
			std::cout << "(P: (" << point << "))" << std::endl;
	}

	//Print the points in the given range. Default parameter values are initialized as the layers are traversed
	void printRange(Point2D* lowerLeft, Point2D* upperRight,
			double leftBound = NaN, double rightBound = NaN, double lowerBound = NaN, double upperBound = NaN) {
		//If the search area is bounded by all 4 lines, and the range is inside this, print all the subtree (black nodes)
		if(!std::isnan(leftBound) && !std::isnan(rightBound) && !std::isnan(lowerBound) && !std::isnan(upperBound)) {
			Point2D* lowerLeftBound = new Point2D(leftBound, lowerBound);
			Point2D* upperRightBound = new Point2D(rightBound, upperBound);
			bool inside = lowerLeftBound->inside(lowerLeft, upperRight) && upperRightBound->inside(lowerLeft, upperRight);
			delete upperRightBound;
			delete lowerLeftBound;

			if(inside) {
				displayPoints();
				return;
			}
		}

		if(dimension == 0) { //Gray and white nodes, delve into subtrees depending on their median values
			if(compareDoubles(lowerLeft->x, median) < 1)
				left->printRange(lowerLeft, upperRight, leftBound, median, lowerBound, upperBound);
			if(compareDoubles(upperRight->x, median) > 0)
				right->printRange(lowerLeft, upperRight, median, rightBound, lowerBound, upperBound);
		} else if(dimension == 1) {
			if(compareDoubles(lowerLeft->y, median) < 1)
				left->printRange(lowerLeft, upperRight, leftBound, rightBound, lowerBound, median);
			if(compareDoubles(upperRight->y, median) > 0)
				right->printRange(lowerLeft, upperRight, leftBound, rightBound, median, upperBound);
		} else if(dimension == -1 && point->inside(lowerLeft, upperRight)) //If leaf and inside, print
			std::cout << point << std::endl;
	}
};

//Construct a subtree for given parent node, using the point vector and given range on the vector
KDTree* buildKDtree(std::vector<Point2D*> &points, int begin, int end, KDTree* parent = nullptr) {
	if(begin != end) { //Means internal node
		int middle = (begin + end) / 2;
		int dimension = parent ? (parent->dimension + 1) % 2 : 0;

		//This is the core of the algorithm. nth_elements finds the nth smallest value and partitions the vector around it
		//The lambda is the comparison function, that tells the nth_element to compare points by the given dimension
		std::nth_element(points.begin() + begin, points.begin() + middle, points.begin() + end + 1,
			[dimension] (Point2D* point1, Point2D* point2) {
				return point1->compareDimension(point2, dimension) < 0;
		}); //At the end, middle index is the median, left side is smaller, right side is larger. Takes O(n) on average
		double median = points.at(middle)->get(dimension);
		//Recursively construct subtrees, split by the median of the dimension
		KDTree* node = new KDTree(dimension, median, nullptr, parent);
		node->left = buildKDtree(points, begin, middle, node);
		node->right = buildKDtree(points, middle + 1, end, node);
		return node;
	} else //If leaf node, simply create a node
		return new KDTree(-1, NaN, points.at(begin), parent);
}

//Construct the KDTree from given points in a file
KDTree* buildKDtree(std::string filename) {
	std::ifstream file(filename);
	std::vector<Point2D*> points;
	double x, y;
	//Read all the points and fill the vector
	while(true) {
		file >> x >> y;

		if(file.eof())
			break;
		else
			points.emplace_back(new Point2D(x, y));
	}
	//Call helper function with constructed vector and default parent value, meaning root
	return buildKDtree(points, 0, points.size() - 1);
}

int main(void) { //Nothing to see here, just the processing of directives and invocations of needed methods
	double x1, y1, x2, y2;
	std::string string;
	Point2D *point1, *point2;
	KDTree* tree = nullptr; //There be KDTree!

	while(true) {
		std::cin >> string;
		if(!string.compare("build-kdtree")) {
			std::cin >> string;
			tree = buildKDtree(string);
		} else if(!string.compare("insert")) {
			std::cin >> x1 >> y1;
			point1 = new Point2D(x1, y1);
			tree->insert(point1);
			std::cout << "Inserted " << point1 << "\n" << std::endl;
		} else if(!string.compare("remove")) {
			std::cin >> x1 >> y1;
			point1 = new Point2D(x1, y1);
			if(tree->search(point1)) { //Search is only required to print "Not found"
				tree->remove(point1); //And remove would suffice if the function signatures could be altered
				std::cout << "Removed " << point1 << "\n" << std::endl;
			} else
				std::cout << "Not found " << point1 << "\n" << std::endl;
			delete point1;
		} else if(!string.compare("search")) {
			std::cin >> x1 >> y1;
			point1 = new Point2D(x1, y1);
			std::cout << (tree->search(point1) ? "Found " : "Not found ") << point1 << "\n" << std::endl;
			delete point1;
		} else if(!string.compare("display-tree")) {
			tree->displayTree(); //Called with the default parameter value, meaning root
			std::cout << std::endl;
		} else if(!string.compare("display-points")) {
			tree->displayPoints();
			std::cout << std::endl;
		} else if(!string.compare("find-min-x")) {
			std::cout << "minimum-x is " << tree->findMin(0) << "\n" << std::endl;
		} else if(!string.compare("find-min-y")) {
			std::cout << "minimum-y is " << tree->findMin(1) << "\n" << std::endl;
		} else if(!string.compare("find-max-x")) {
			std::cout << "maximum-x is " << tree->findMax(0) << "\n" << std::endl;
		} else if(!string.compare("find-max-y")) {
			std::cout << "maximum-y is " << tree->findMax(1) << "\n" << std::endl;
		} else if(!string.compare("print-range")) {
			std::cin >> x1 >> y1 >> x2 >> y2;
			point1 = new Point2D(x1, y1);
			point2 = new Point2D(x2, y2);
			tree->printRange(point1, point2);
			delete point2;
			delete point1;
		} else if(!string.compare("quit")) {
			delete tree;
			break;
		} else
			std::cout << "Unrecognized command: " << string << "\n" << std::endl;
	}
}

