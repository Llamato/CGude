in vec3 interpolatedColor;
out vec4 fragmentColor;

void main()
{
	fragmentColor = vec4(interpolatedColor, 1);
}
