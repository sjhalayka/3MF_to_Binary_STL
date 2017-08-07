// Public domain software
// Shawn Halayka - sjhalayka@gmail.com



// g++ *.cpp -I/usr/local/Cellar/libzip/1.1.2/include -I/usr/local/Cellar/libzip/1.1.2/lib/libzip/include -L/usr/local/Cellar/libzip/1.1.2/lib -lzip




#include <zip.h>
#include "tinyxml2.h"
#include "main.h"
using namespace tinyxml2;

int main(int argc, char **argv)
{
	if(3 != argc)
	{
		cout << "Usage: " << argv[0] << " input.3mf output.stl" << endl;
		return 0;
	}

	size_t s = 0;

	// get char array, automatically terminated with a \0.
    char *v = libzip_zip_extract_archive_file_to_heap_terminate_with_null(argv[1], "3D/3dmodel.model", &s, 0);

	if(0 == v)
        return -1;

	// triangles for output to stl
	vector<triangle> triangles;
	
	// pass xml file as a \0 terminated c-string
	XMLDocument doc;
	doc.Parse(v);
	
	// Release memory related to \0 terminated c-string
    delete [] v;

	// Parse the XML file
	XMLElement* model = doc.FirstChildElement("model");

	if(model)
	{
		XMLElement* resources = model->FirstChildElement("resources");

		if(resources)
		{
			XMLElement* object = resources->FirstChildElement("object");

			while(object)
			{
				XMLElement *mesh = object->FirstChildElement("mesh");

				if(mesh)
				{
					vector<vertex_3> vertices;

					XMLElement *verts = mesh->FirstChildElement("vertices");

					if(verts)
					{
						XMLElement* vertex_child = verts->FirstChildElement("vertex");

						while(vertex_child)
						{
							vertex_3 vertex;

							istringstream iss;
                            iss.str(vertex_child->Attribute("x"));
							iss >> vertex.x;
							
							iss.clear();
							iss.str(vertex_child->Attribute("y"));
							iss >> vertex.y;
							
							iss.clear();
							iss.str(vertex_child->Attribute("z"));
							iss >> vertex.z;

							vertices.push_back(vertex);

							vertex_child = vertex_child->NextSiblingElement("vertex");
						}
					}

					XMLElement *tris = mesh->FirstChildElement("triangles");

					if(tris)
					{
						XMLElement* tris_child = tris->FirstChildElement("triangle");

						while(tris_child)
						{
							triangle tri;
							size_t index;

							istringstream iss;
							iss.str(tris_child->Attribute("v1"));
							iss >> index;
							tri.vertex[0] = vertices[index];

							iss.clear();
							iss.str(tris_child->Attribute("v2"));
							iss >> index;
							tri.vertex[1] = vertices[index];

							iss.clear();
							iss.str(tris_child->Attribute("v3"));
							iss >> index;
							tri.vertex[2] = vertices[index];

							triangles.push_back(tri);

							tris_child = tris_child->NextSiblingElement("triangle");
						}
					}
				}

				object = object->NextSiblingElement( "object" );
			}
		}
	}

	cout << "Read " << triangles.size() << " triangles..." << endl;

	// Write triangles to STL file...
	save_to_binary_stereo_lithography_file(triangles, argv[2], 65535);
//    save_to_ASCII_stereo_lithography_file(triangles, argv[2]);
    
    
	return 1;
}



