#pragma once

class Mesh
{
public:
	Mesh(const char* filename);
	std::vector<float3> GetVertices() const { return vertices; }
private:
	//Change the way mesh/models work so no dupplicate vertices have to be stored
	std::vector<float3> vertices;
};