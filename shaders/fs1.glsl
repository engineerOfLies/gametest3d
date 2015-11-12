#version 330
in vec4 theColor;
out vec4 outputColor;
void main()
{
    vec4 newcolor = theColor;
    newcolor.x *=3;
	outputColor = newcolor;
}
