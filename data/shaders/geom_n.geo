#version 400

layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

in vec4 color[];
in vec3 normal[];

uniform mat4 model;

out vec4 vColor;
out vec2 UV;

void main()
{
    for (int i = 0; i < gl_in.length(); i++)
    {        
        gl_Position = model * gl_in[i].gl_Position;
        vColor = color[i];
        EmitVertex();
        
        gl_Position = model * vec4(gl_in[i].gl_Position.xyz + normal[i] * 0.3, 1.0);
        vColor = color[i];
        EmitVertex();
        
        EndPrimitive();
    }
}