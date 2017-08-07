// #include <sys/stat.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <cstring>
#include <ios>
#include <cmath>
#include <set>
#include <sstream>
#include <iomanip>
#include <string>
using std::string;
using std::setprecision;
using std::ostringstream;
using std::istringstream;
using std::set;
using std::sqrt;
using std::memcpy;
using std::vector;
using std::cout;
using std::endl;
using std::ofstream;
using std::ifstream;
using std::ios_base;
using std::hex;


class vertex_3
{
public:
	float x;
	float y;
	float z;

	vertex_3(void)
	{
		x = y = z = 0;
	}


	inline const vertex_3& operator+(const vertex_3 &right) const
	{
		static vertex_3 temp;

		temp.x = this->x + right.x;
		temp.y = this->y + right.y;
		temp.z = this->z + right.z;

		return temp;
	}



	inline bool operator==(const vertex_3 &right) const
	{
		if (right.x == x && right.y == y && right.z == z)
			return true;
		else
			return false;
	}

	inline bool operator<(const vertex_3 &right) const
	{
		if(x < right.x)
			return true;
		else if(x > right.x)
			return false;

		if(y < right.y)
			return true;
		else if(y > right.y)
			return false;

		if(z < right.z)
			return true;
		else if(z > right.z)
			return false;

		return false;
	}

	inline const vertex_3& operator-(const vertex_3 &right) const
	{
		static vertex_3 temp;

		temp.x = this->x - right.x;
		temp.y = this->y - right.y;
		temp.z = this->z - right.z;

		return temp;
	}

	inline const float dot(vertex_3 &rhs) const
	{
		return x*rhs.x + y*rhs.y + z*rhs.z;
	}

	inline const float self_dot(void) const
	{
		return x*x + y*y + z*z;
	}

	inline const float length(void) const
	{
		return sqrt(self_dot());
	}


	inline const void normalize(void)
	{
		float len = length();

		if (0.0f != len)
		{
			x /= len;
			y /= len;
			z /= len;
		}
	}

	inline const vertex_3& cross(const vertex_3 &right) const
	{
		static vertex_3 temp;

		temp.x = y*right.z - z*right.y;
		temp.y = z*right.x - x*right.z;
		temp.z = x*right.y - y*right.x;

		return temp;
	}



};




class triangle
{
public:

	triangle(void)
	{
		vertex[0].x = 0; vertex[0].y = 0; vertex[0].z = 0;
		vertex[1].x = 0; vertex[1].y = 0; vertex[1].z = 0;
		vertex[2].x = 0; vertex[2].y = 0; vertex[2].z = 0;
	}

	vertex_3 vertex[3];
};




bool load_from_stl(vector<triangle> &triangles, const char* const file_name)
{
	triangles.clear();

	// Read from file.
	ifstream in(file_name, ios_base::binary);

	if (in.fail())
		return false;

	const size_t header_size = 80;
	vector<char> buffer(header_size, 0);
	unsigned int num_triangles = 0; // Must be 4-byte unsigned int.

	// Read header.
	in.read(reinterpret_cast<char *>(&(buffer[0])), header_size);

    if (header_size != in.gcount())
		return false;

	// If ASCII text file header, abort
	if ('s' == tolower(buffer[0]) &&
		'o' == tolower(buffer[1]) &&
		'l' == tolower(buffer[2]) &&
		'i' == tolower(buffer[3]) &&
		'd' == tolower(buffer[4]))
	{
		cout << "Encountered ASCII STL file header -- aborting" << endl;
		return false;
	}



	// Read number of triangles.
	in.read(reinterpret_cast<char *>(&num_triangles), sizeof(unsigned int));

	if (sizeof(unsigned int) != in.gcount())
		return false;

	triangles.resize(num_triangles);

	// Enough bytes for twelve 4-byte floats plus one 2-byte integer, per triangle.
	const size_t data_size = (12 * sizeof(float)+sizeof(short unsigned int)) * num_triangles;
	buffer.resize(data_size, 0);

	in.read(reinterpret_cast<char *>(&buffer[0]), data_size);

	if (data_size != in.gcount())
		return false;

	// Use a pointer to assist with the copying.
	// Should probably use std::copy() instead, but memcpy() does the trick, so whatever...
	char *cp = &buffer[0];

	for (vector<triangle>::iterator i = triangles.begin(); i != triangles.end(); i++)
	{
		// Skip face normal.
		cp += 3 * sizeof(float);

		// Get vertices.
		memcpy(&i->vertex[0].x, cp, sizeof(float)); cp += sizeof(float);
		memcpy(&i->vertex[0].y, cp, sizeof(float)); cp += sizeof(float);
		memcpy(&i->vertex[0].z, cp, sizeof(float)); cp += sizeof(float);
		memcpy(&i->vertex[1].x, cp, sizeof(float)); cp += sizeof(float);
		memcpy(&i->vertex[1].y, cp, sizeof(float)); cp += sizeof(float);
		memcpy(&i->vertex[1].z, cp, sizeof(float)); cp += sizeof(float);
		memcpy(&i->vertex[2].x, cp, sizeof(float)); cp += sizeof(float);
		memcpy(&i->vertex[2].y, cp, sizeof(float)); cp += sizeof(float);
		memcpy(&i->vertex[2].z, cp, sizeof(float)); cp += sizeof(float);

		// Skip attribute.
		cp += sizeof(short unsigned int);
	}

	in.close();

	return true;
}

bool save_to_binary_stereo_lithography_file(const vector<triangle> triangles, const char *const file_name, const size_t buffer_width)
{
	if(0 == triangles.size())
		return false;

	// Write to file.
	ofstream out(file_name, ios_base::binary);

	if(out.fail())
		return false;

	const size_t header_size = 80;
	vector<char> buffer(header_size, 0);
	const unsigned int num_triangles = triangles.size(); // Must be 4-byte unsigned int.
	vertex_3 normal;

	// Write blank header.
	out.write(reinterpret_cast<const char *>(&(buffer[0])), header_size);

	// Write number of triangles.
	out.write(reinterpret_cast<const char *>(&num_triangles), sizeof(unsigned int));

	// Enough bytes for twelve 4-byte floats plus one 2-byte integer, per triangle.
	const size_t per_triangle_data_size = (12*sizeof(float) + sizeof(short unsigned int));
	const size_t buffer_size = per_triangle_data_size * buffer_width;
	buffer.resize(buffer_size, 0);

	// Use a pointer to assist with the copying.
	// Should probably use std::copy() instead, but memcpy() does the trick, so whatever...
	char *cp = &buffer[0];
	size_t buffer_count = 0;

	cout << "Writing " << per_triangle_data_size*triangles.size() / 1048576 << " MB of data to disk" << endl;

	for(size_t i = 0; i < triangles.size(); i++)
	{
		memcpy(cp, &normal.x, sizeof(float)); cp += sizeof(float);
		memcpy(cp, &normal.y, sizeof(float)); cp += sizeof(float);
		memcpy(cp, &normal.z, sizeof(float)); cp += sizeof(float);

		memcpy(cp, &triangles[i].vertex[0].x, sizeof(float)); cp += sizeof(float);
		memcpy(cp, &triangles[i].vertex[0].y, sizeof(float)); cp += sizeof(float);
		memcpy(cp, &triangles[i].vertex[0].z, sizeof(float)); cp += sizeof(float);
		memcpy(cp, &triangles[i].vertex[1].x, sizeof(float)); cp += sizeof(float);
		memcpy(cp, &triangles[i].vertex[1].y, sizeof(float)); cp += sizeof(float);
		memcpy(cp, &triangles[i].vertex[1].z, sizeof(float)); cp += sizeof(float);
		memcpy(cp, &triangles[i].vertex[2].x, sizeof(float)); cp += sizeof(float);
		memcpy(cp, &triangles[i].vertex[2].y, sizeof(float)); cp += sizeof(float);
		memcpy(cp, &triangles[i].vertex[2].z, sizeof(float)); cp += sizeof(float);

		cp += sizeof(short unsigned int);

		buffer_count++;

		// If buffer is full, write triangles in buffer to disk.
		if(buffer_count == buffer_width)
		{
			out.write(reinterpret_cast<const char *>(&buffer[0]), buffer_size);

			if(out.fail())
				return false;

			buffer_count = 0;
			cp = &buffer[0];
		}
	}

	// Write any remaining triangles in buffer to disk.
	// This will occur whenever triangles.size() % buffer_width != 0
	// (ie. when triangle count is not a multiple of buffer_width, which should happen almost all of the time).
	if(buffer_count > 0)
	{
		out.write(reinterpret_cast<const char *>(&buffer[0]), per_triangle_data_size*buffer_count);

		if(out.fail())
			return false;
	}

	out.close();

	return true;
} 