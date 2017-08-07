#include "miniz.h"
#include "tinyxml.h"
#include "main.h"



int main(int argc, char **argv)
{
	if(3 != argc)
	{
		cout << "Usage: " << argv[0] << " input.3mf output.stl" << endl;
		return 0;
	}

	size_t s = 0;

	// get char array, not terminated with a \0.
	char *v = static_cast<char *>(mz_zip_extract_archive_file_to_heap(argv[1], "3D/3dmodel.model", &s, 0));

	if(0 == v)
	{
		cout << "File not found" << endl;
		return -1;
	}		

	// convert char array to string, to be terminated with a \0 when .c_str() is called later.
	string v2;

	for(size_t i = 0; i < s; i++)
		v2 += v[i];

	// release memory related to char array
	mz_free(v);



	// triangles for output to stl
	vector<triangle> triangles;
	
	// pass xml file as a \0 terminated c-string
	TiXmlDocument doc;
	doc.Parse(v2.c_str());
	
	TiXmlElement* model = doc.FirstChildElement("model");

	if(model)
	{
		TiXmlElement* resources = model->FirstChildElement("resources");

		if(resources)
		{
			TiXmlElement* object = resources->FirstChildElement("object");

			while(object)
			{
				TiXmlElement *mesh = object->FirstChildElement("mesh");

				if(mesh)
				{
					vector<vertex_3> vertices;

					TiXmlElement *verts = mesh->FirstChildElement("vertices");

					if(verts)
					{
						TiXmlElement* vertex_child = verts->FirstChildElement("vertex");

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

					TiXmlElement *tris = mesh->FirstChildElement("triangles");

					if(tris)
					{
						TiXmlElement* tris_child = tris->FirstChildElement("triangle");

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
	save_to_binary_stereo_lithography_file(triangles, argv[2], 4);

	return 1;
}



