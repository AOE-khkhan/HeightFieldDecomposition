#include "heightfieldslist.h"
#include "common.h"

using namespace cg3;

HeightfieldsList::HeightfieldsList() : nVisible(-1) {
}

void HeightfieldsList::draw() const {
    if (nVisible < 0){
        for (unsigned int i = 0; i < heightfields.size(); ++i){
            heightfields[i].draw();
        }
    }
    else {
        heightfields[nVisible].draw();
    }
}

Point3d HeightfieldsList::sceneCenter() const{
	BoundingBox3 bb(Point3d(-1,-1,-1), Point3d(1,1,1));
    for (unsigned int i = 0; i < heightfields.size(); i++){
		bb.min() = bb.min().min(heightfields[i].boundingBox().min());
		bb.max() = bb.max().max(heightfields[i].boundingBox().max());
    }
    return bb.center();
}

double HeightfieldsList::sceneRadius() const {
	BoundingBox3 bb(Point3d(-1,-1,-1), Point3d(1,1,1));
    for (unsigned int i = 0; i < heightfields.size(); i++){
		bb.min() = bb.min().min(heightfields[i].boundingBox().min());
		bb.max() = bb.max().max(heightfields[i].boundingBox().max());
    }
    return bb.diag() / 2;
}

void HeightfieldsList::setVisibleHeightfield(int i) {
    assert (i < (int)heightfields.size());
    nVisible = i;
}

void HeightfieldsList::resize(int n) {
    heightfields.resize(n);
    targets.resize(n);
}

unsigned int HeightfieldsList::getNumberVerticesHeightfield(int i) const {
    assert (i < (int)heightfields.size());
	return heightfields[i].numberVertices();
}

Point3d HeightfieldsList::getVertexOfHeightfield(int he, int v) const {
    assert (he < (int)heightfields.size());
	return heightfields[he].vertex(v);
}

Vec3d HeightfieldsList::getTarget(int i) const {
    assert(i < (int)heightfields.size());
    return targets[i];
}

void HeightfieldsList::setWireframe(bool b) {
    for (unsigned int i = 0; i < heightfields.size(); ++i){
        heightfields[i].setWireframe(b);
    }
}

void HeightfieldsList::setPointShading() {
    for (unsigned int i = 0; i < heightfields.size(); ++i){
        heightfields[i].setPointsShading();
    }
}

void HeightfieldsList::setFlatShading() {
    for (unsigned int i = 0; i < heightfields.size(); ++i){
        heightfields[i].setFlatShading();
    }
}

void HeightfieldsList::setSmoothShading() {
    for (unsigned int i = 0; i < heightfields.size(); ++i){
        heightfields[i].setSmoothShading();
    }
}

void HeightfieldsList::checkHeightfields() const {
    for (unsigned int i = 0; i < heightfields.size(); i++){
        const DrawableEigenMesh& m = heightfields[i];
		for (unsigned int f = 0; f < m.numberFaces(); f++){
			if (m.faceNormal(f).dot(targets[i]) < FLIP_ANGLE-CG3_EPSILON && m.faceNormal(f).dot(targets[i]) > -1 + CG3_EPSILON){
				std::cerr << "Hieghtfield: " << i << "; Triangle: " << f << "; Flip: " << m.faceNormal(f).dot(targets[i]) << "\n";
            }
        }
    }
}

void HeightfieldsList::rotate(const Eigen::MatrixXd m) {
    for (unsigned int i = 0; i < heightfields.size(); i++){
        heightfields[i].rotate(m);
        targets[i].rotate(m);
        targets[i].normalize();
    }
}

void HeightfieldsList::addHeightfield(const DrawableEigenMesh& m, const Vec3d& target, int i, bool updateColor) {
    if (i < 0){
        heightfields.push_back(m);
        targets.push_back(target);
        if (updateColor){
            Color c = colorOfNormal(target);
            heightfields[heightfields.size()-1].setFaceColor(c.redF(), c.greenF(), c.blueF());
			for (unsigned int i = 0; i < m.numberFaces(); i++){
				if (m.faceNormal(i).dot(target) < FLIP_ANGLE-CG3_EPSILON)
                    heightfields[heightfields.size()-1].setFaceColor(0,0,0,i);
            }
        }
    }
    else {
        heightfields[i] = m;
        targets[i] = target;
        if (updateColor) {
            Color c = colorOfNormal(target);
            heightfields[i].setFaceColor(c.redF(), c.greenF(), c.blueF());
			for (unsigned int j = 0; j < m.numberFaces(); j++){
				if (m.faceNormal(j).dot(target) < FLIP_ANGLE-CG3_EPSILON)
                    heightfields[i].setFaceColor(0,0,0,j);
            }
        }
    }
}

unsigned int HeightfieldsList::getNumHeightfields() const {
    return heightfields.size();
}

void HeightfieldsList::removeHeightfield(unsigned int i) {
    assert (i < heightfields.size());
    heightfields.erase(heightfields.begin()+i);
    targets.erase(targets.begin()+i);
}

const cg3::EigenMesh& HeightfieldsList::getHeightfield(unsigned int i) const {
    assert (i < heightfields.size());
    return heightfields[i];
}

cg3::EigenMesh& HeightfieldsList::getHeightfield(unsigned int i) {
    assert (i < heightfields.size());
    return heightfields[i];
}

void HeightfieldsList::setHeightfield(const cg3::EigenMesh& m, unsigned int i, bool updateColor) {
    assert (i < heightfields.size());
    heightfields[i] = m;
    if (updateColor){
        Color c = colorOfNormal(targets[i]);
        heightfields[i].setFaceColor(c.redF(), c.greenF(), c.blueF());
		for (unsigned int j = 0; j < m.numberFaces(); j++){
			if (m.faceNormal(j).dot(targets[i]) < FLIP_ANGLE-CG3_EPSILON)
                heightfields[i].setFaceColor(0,0,0,j);
        }
    }
}

void HeightfieldsList::insertHeightfield(const cg3::EigenMesh& m, const Vec3d& target, unsigned int i, bool updateColor) {
    assert (i < heightfields.size()+1);
    heightfields.insert(heightfields.begin() + i, m);
    targets.insert(targets.begin() + i, target);
    if (updateColor){
        Color c = colorOfNormal(target);
        heightfields[i].setFaceColor(c.redF(), c.greenF(), c.blueF());
		for (unsigned int j = 0; j < m.numberFaces(); j++){
			if (m.faceNormal(j).dot(target) < FLIP_ANGLE-CG3_EPSILON)
                heightfields[i].setFaceColor(0,0,0,j);
        }
    }
}

void HeightfieldsList::explode(const Point3d& bc, double dist) {
    for (unsigned int i = 0; i < heightfields.size(); ++i) {
		Point3d translation;
		Vec3d v = heightfields[i].barycenter() - bc;
        v.normalize();
        translation = v * dist;
        heightfields[i].translate(translation);
    }
}

void HeightfieldsList::serialize(std::ofstream& binaryFile) const {
    serializeObjectAttributes("HeightFieldList", binaryFile, heightfields, targets);
}

void HeightfieldsList::deserialize(std::ifstream& binaryFile) {
    deserializeObjectAttributes("HeightFieldList", binaryFile, heightfields, targets);
    nVisible = -1;
}
