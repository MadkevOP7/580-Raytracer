#pragma once
#include <cmath>
#include <vector>
#include <unordered_map>
#include <string>
//Defines
#define RT_SUCCESS      0
#define RT_FAILURE      1
#define MAX_DEPTH        5
#define M_PI 3.1415

const std::string ASSETS_PATH = "Assets/";

class Raytracer {

	//State machine
public:

	struct Material;
	struct Vector3;
	struct Vector2;
	struct Matrix;
	struct Pixel;
	struct Display;
	struct RaycastHitInfo;
	struct Ray;
	struct Vertex;
	struct Triangle;
	struct Mesh;
	struct Camera;
	struct Light;
	struct Transformation;
	struct Shape;
	struct Scene;

	//Structures
	struct Vector3 {
		union {
			struct {
				float x, y, z;

			};
			float v[3];
		};

		Vector3() : x(0.0f), y(0.0f), z(0.0f) {}

		// Constructor for initializing Vector3 from individual x, y, and z values
		Vector3(float xVal, float yVal, float zVal) : x(xVal), y(yVal), z(zVal) {}

		// Constructor for initializing Vector3 from an array of 3 values
		Vector3(const float values[3]) : x(values[0]), y(values[1]), z(values[2]) {}

		Vector3 operator/(const float scalar) const {
			return { x / scalar, y / scalar, z / scalar };
		}

		Vector3 operator*(const float scalar) const {
			return { x * scalar, y * scalar, z * scalar };
		}

		//Component wise multiplication
		Vector3 operator*(const Vector3& other) const {
			return { x * other.x, y * other.y, z * other.z };
		}

		// Vector subtraction
		Vector3 operator-(const Vector3& other) const {
			return { x - other.x, y - other.y, z - other.z };
		}

		Vector3 operator+(const Vector3& other) const {
			return { x + other.x, y + other.y, z + other.z };
		}

		// Normalize the vector
		void normalize() {
			float length = std::sqrt(x * x + y * y + z * z);
			x /= length; y /= length; z /= length;
		}

		// Cross product
		static Vector3 cross(const Vector3& a, const Vector3& b) {
			return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
		}

		Vector3 cross(const Vector3& other) {
			return { y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x };
		}

		// Dot product
		static float dot(const Vector3& a, const Vector3& b) {
			return a.x * b.x + a.y * b.y + a.z * b.z;
		}

		float dot(const Vector3& other) {
			return x * other.x + y * other.y + z * other.z;
		}

		float dot(const Vector3& other) const {
			return x * other.x + y * other.y + z * other.z;
		}

		static Vector3 reflect(const Vector3& I, const Vector3& N) {
			float IDotN = I.dot(N);
			IDotN *= 2;
			Vector3 _N = N * IDotN;
			return I - _N;
		}
	};

	struct Vector2 {
		union {
			struct {
				float x, y;
			};
			float v[2];
		};
		//Default constructor
		Vector2() : x(0.0f), y(0.0f) {}

		// Constructor for initializing Vector2 from individual x and y values
		Vector2(float xVal, float yVal) : x(xVal), y(yVal) {}

		// Constructor for initializing Vector2 from an array of 2 values
		Vector2(const float values[2]) : x(values[0]), y(values[1]) {}
	};

	struct Matrix {
		float m[4][4];

		// Overload the subscript operator to allow direct access to internal array
		float* operator[](int index) {
			return m[index];
		}
		const float* operator[](int index) const {
			return m[index];
		}

		Matrix operator*(const Matrix& other) const {
			Matrix result;
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					result.m[i][j] = 0;
					for (int k = 0; k < 4; ++k) {
						result.m[i][j] += this->m[i][k] * other.m[k][j];
					}
				}
			}
			return result;
		}

		Matrix operator+(const Matrix& other) const {
			Matrix result;
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					result.m[i][j] = this->m[i][j] + other.m[i][j];
				}
			}
			return result;
		}

		Matrix operator-(const Matrix& other) const {
			Matrix result;
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					result.m[i][j] = this->m[i][j] - other.m[i][j];
				}
			}
			return result;
		}

		Matrix transpose() const {
			Matrix result;
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					result.m[i][j] = this->m[j][i];
				}
			}
			return result;
		}

		Vector3 TransformPoint(const Vector3& point) const {
			float x = m[0][0] * point.x + m[0][1] * point.y + m[0][2] * point.z + m[0][3];
			float y = m[1][0] * point.x + m[1][1] * point.y + m[1][2] * point.z + m[1][3];
			float z = m[2][0] * point.x + m[2][1] * point.y + m[2][2] * point.z + m[2][3];
			return Vector3(x, y, z);
		}

		//Inverse
		static float Determinant3x3(const Matrix& matrix) {
			return matrix.m[0][0] * (matrix.m[1][1] * matrix.m[2][2] - matrix.m[1][2] * matrix.m[2][1]) -
				matrix.m[0][1] * (matrix.m[1][0] * matrix.m[2][2] - matrix.m[1][2] * matrix.m[2][0]) +
				matrix.m[0][2] * (matrix.m[1][0] * matrix.m[2][1] - matrix.m[1][1] * matrix.m[2][0]);
		}

		static float Determinant(const Matrix& matrix) {
			float det = 0;
			for (int i = 0; i < 4; i++) {
				Matrix submatrix;
				for (int j = 1; j < 4; j++) {
					for (int k = 0; k < 4; k++) {
						if (k < i) {
							submatrix.m[j - 1][k] = matrix.m[j][k];
						}
						else if (k > i) {
							submatrix.m[j - 1][k - 1] = matrix.m[j][k];
						}
					}
				}
				det += (i % 2 == 0 ? 1 : -1) * matrix.m[0][i] * Determinant3x3(submatrix);
			}
			return det;
		}

		static void Adjoint(const Matrix& matrix, Matrix& adjoint) {
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					Matrix submatrix;
					int subi = 0;
					for (int k = 0; k < 4; k++) {
						if (k == i) continue;
						int subj = 0;
						for (int l = 0; l < 4; l++) {
							if (l == j) continue;
							submatrix.m[subi][subj] = matrix.m[k][l];
							subj++;
						}
						subi++;
					}
					float cofactor = Determinant3x3(submatrix);
					if ((i + j) % 2 != 0) cofactor = -cofactor;
					adjoint.m[j][i] = cofactor;  // Transpose the cofactor matrix
				}
			}
		}

		static int InverseAndTranspose(const Matrix& matrix, Matrix& result) {
			float det = Determinant(matrix);
			if (fabs(det) < 1e-10) {
				return RT_FAILURE;
			}

			Matrix adjoint;
			Adjoint(matrix, adjoint);
			Matrix invMatrix;
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					invMatrix.m[i][j] = adjoint.m[i][j] / det;
				}
			}

			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					result.m[i][j] = invMatrix.m[j][i];
				}
			}

			// Ensure the rest of the matrix (the translation part) is also copied/transformed
			for (int i = 0; i < 4; i++) {
				result.m[3][i] = invMatrix.m[3][i]; // Copy the translation row
				result.m[i][3] = invMatrix.m[i][3]; // Copy the translation column
			}
			result.m[3][3] = 1.0f; // Set the homogeneous coordinate to 1

			return RT_SUCCESS;
		}
	};

	struct Pixel {
		short r, g, b;
	};

	struct Display {
		Pixel* frameBuffer;
		int xRes, yRes;
	};

	struct Material {
		Vector3 surfaceColor;
		float Ka;
		float Kd;
		float Ks;
		float Kt;
		float specularExponet;
		std::string textureId;
		bool reflective;  // Add a flag to indicate if the material is reflective
		float reflectionStrength;  // Add a property for reflection strength
	};

	struct RaycastHitInfo {
		Vector3 hitPoint;
		Vector3 normal;
		float distance;
		Material material;
	};

	struct Ray {
		Vector3 origin; // World space
		Vector3 direction; // Normalized

		Ray(const Vector3& o, const Vector3& d) : origin(o), direction(d) {}
	};

	struct Vertex {
		Vector3 vertexPos;
		Vector3 vertexNormal;
		Vector2 texture;
	};

	struct Triangle {
		Vertex v0;
		Vertex v1;
		Vertex v2;
	};

	struct Mesh {
		std::vector<Triangle> triangles;
	};

	//Todo: need this later for generating ray
	//Perspective & Camera
	struct Camera
	{
		Matrix        viewMatrix;		/* world to image space */
		Matrix        projectMatrix;  /* perspective projection */
		Vector3 viewDirection;
		Vector3 from;
		Vector3 to;
		float near, far, right, left, top, bottom;
		int xRes;
		int yRes;
	};

	//Scene
	struct Light {
		Vector3 color;
		float intensity;

		Vector3 direction;
	};

	struct Transformation {
		Vector3 scale = Vector3(1, 1, 1);
		//Each x, y, z is rotation around that axis in degrees
		Vector3 rotation;
		Vector3 translation;
	};

	struct Shape
	{
		std::string id;
		std::string geometryId;
		std::string notes;
		Material material;
		Transformation transforms;
	};

	struct Scene
	{
		std::vector<Shape> shapes;
		Camera camera;
		std::unordered_map<std::string, Mesh*> meshMap;
		std::vector<Light> lights;
		Light directional;
		Light ambie;
	};

	bool NearlyEquals(float a, float b);

	//This is the main function against the whole scene, but we will need some helper raycast
	//Returns true if intersects with an object
	bool Raycast(Ray& ray, RaycastHitInfo& hitInfo, int depth);


	//Helper ray cast functions
	//M�ller�Trumbore intersection algorithm
	bool RaycastTriangle(Ray& ray, Triangle& triangle, RaycastHitInfo& hitInfo, Matrix& modelMatrix);
	int LoadMesh(const std::string meshName);
	int LoadSceneJSON(const std::string scenePath);
	Matrix ComputeModelMatrix(const Transformation& transform);
	Vector3 MixColors(Vector3 color1, Vector3 color2, float blendFactor);

	int FlushFrameBufferToPPM(std::string outputName);

	//Constructor
	Raytracer(int width, int height);
private:
	const float EPSILON = 0.00001f;
	Scene* mScene = nullptr;
	Display* mDisplay;
};
