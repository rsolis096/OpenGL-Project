#include "Model.h"
#include "Object.h"

class SceneManager
{
public:
	vector<Object> scenePrimitives;
	vector<Model> sceneModels;
	//light source vector
private:
	void addObject();
	void deleteObject();
	void getObject();
};