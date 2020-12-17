#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(60);
	ofSetWindowTitle("openFrameworks");

	ofBackground(239);
	ofEnableDepthTest();
	ofSetLineWidth(3);

	this->cap_size = cv::Size(960, 540);

	this->image.allocate(this->cap_size.width, this->cap_size.height, OF_IMAGE_COLOR);
	this->frame = cv::Mat(cv::Size(this->image.getWidth(), this->image.getHeight()), CV_MAKETYPE(CV_8UC3, this->image.getPixels().getNumChannels()), this->image.getPixels().getData(), 0);

	this->cap.open("D:\\MP4\\video11.mp4");
	this->number_of_frames = this->cap.get(cv::CAP_PROP_FRAME_COUNT);
	for (int i = 0; i < this->number_of_frames; i++) {

		cv::Mat src, tmp;
		this->cap >> src;
		if (src.empty()) {

			continue;
		}

		cv::resize(src, tmp, this->cap_size);
		cv::cvtColor(tmp, tmp, cv::COLOR_BGR2RGB);

		this->frame_list.push_back(tmp);
	}
}

//--------------------------------------------------------------
void ofApp::update() {

	ofSeedRandom(39);

	int frame_index = ofGetFrameNum() % this->number_of_frames;
	this->frame_list[frame_index].copyTo(this->frame);
	this->image.update();

	this->face_list.clear();
	this->line_list.clear();

	float radius = 250;
	int deg_span = 3;
	int x_span = 60;
	for (int x = 0; x < this->cap_size.width; x += x_span) {

		ofMesh mesh, line;
		line.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINES);
		float tmp_radius = radius;
		for (int deg = 0; deg < 180; deg += deg_span) {

			int start_index = 0;
			if (deg % 30 == 0) {

				auto noise_value = ofNoise(ofRandom(1000), ofGetFrameNum() * 0.01);
				tmp_radius = noise_value > 0.65 ? ofMap(noise_value, 0.65, 1, radius, radius * 2) : radius;

				if (deg > 0) {

					line.addIndex(start_index + 0); line.addIndex(start_index + 3);
					line.addIndex(line.getNumVertices() - 3); line.addIndex(line.getNumVertices() - 2);

					start_index = line.getNumVertices();
				}
			}

			int index = mesh.getNumVertices();
			
			vector<glm::vec3> vertices;
			vertices.push_back(glm::vec3(x - this->cap_size.width * 0.5, tmp_radius * cos(deg * DEG_TO_RAD), tmp_radius * sin(deg * DEG_TO_RAD)));
			vertices.push_back(glm::vec3(x - this->cap_size.width * 0.5, tmp_radius * cos((deg + deg_span) * DEG_TO_RAD), tmp_radius * sin((deg + deg_span) * DEG_TO_RAD)));
			vertices.push_back(glm::vec3(x + x_span - this->cap_size.width * 0.5, tmp_radius* cos((deg + deg_span) * DEG_TO_RAD), tmp_radius * sin((deg + deg_span) * DEG_TO_RAD)));
			vertices.push_back(glm::vec3(x + x_span - this->cap_size.width * 0.5, tmp_radius * cos(deg * DEG_TO_RAD), tmp_radius * sin(deg * DEG_TO_RAD)));

			mesh.addVertices(vertices);
			line.addVertices(vertices);

			int y = ofMap(deg, 0, 180, 0, this->cap_size.height);
			int next_y = ofMap(deg + deg_span, 0, 180, 0, this->cap_size.height);

			mesh.addTexCoord(glm::vec3(x, y, 0));
			mesh.addTexCoord(glm::vec3(x, next_y, 0));
			mesh.addTexCoord(glm::vec3(x + x_span, next_y, 0));
			mesh.addTexCoord(glm::vec3(x + x_span, y, 0));

			mesh.addIndex(index + 0); mesh.addIndex(index + 1); mesh.addIndex(index + 2);
			mesh.addIndex(index + 0); mesh.addIndex(index + 2); mesh.addIndex(index + 3);

			line.addIndex(index + 0); line.addIndex(index + 1);
			line.addIndex(index + 2); line.addIndex(index + 3);
		}

		this->face_list.push_back(mesh);
		this->line_list.push_back(line);
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	this->cam.begin();
	ofRotateY(35 * sin(ofGetFrameNum() * 0.015));
	ofRotateX(35 * sin(ofGetFrameNum() * 0.0075));


	for (int i = 0; i < this->face_list.size(); i++) {

		float z = (this->face_list[i].getVertex(0).z + this->face_list[i].getVertex(2).z) / 2;

		this->image.bind();
		ofSetColor(255);
		this->face_list[i].draw();
		this->image.unbind();

		ofSetColor(39);
		this->line_list[i].drawWireframe();
	}

	this->cam.end();
}

//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(720, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}