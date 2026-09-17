in vec3 vertexPosition;
in vec3 vertexColor;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

out vec3 interpolatedColor;
void main()
{
	interpolatedColor = vertexColor;
	gl_Position = projectionMatrix * modelViewMatrix * vec4(vertexPosition, 1);
}
