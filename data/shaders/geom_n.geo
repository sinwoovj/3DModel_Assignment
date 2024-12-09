#version 400

layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

in vec4 red[];
in vec4 green[];
in vec4 blue[];
in vec3 normal[];
in vec3 tangent[];

uniform mat4 model;

out vec4 vColor;
out vec2 UV;

void main()
{
    for (int i = 0; i < gl_in.length(); i++)
    {     
        //tan
        gl_Position = model * gl_in[i].gl_Position;
        vColor = red[i];
        EmitVertex();
        
        gl_Position = model * vec4(gl_in[i].gl_Position.xyz + normalize(tangent[i]) * 0.3, 1.0);
        vColor = red[i];
        EmitVertex();
        EndPrimitive();

        //Bin
        gl_Position = model * gl_in[i].gl_Position;
        vColor = green[i];
        EmitVertex();
        
        gl_Position = model * vec4(gl_in[i].gl_Position.xyz + normalize(cross(normal[i], tangent[i])) * 0.3, 1.0);
        vColor = green[i];
        EmitVertex();  
        EndPrimitive(); 

        //nor
        gl_Position = model * gl_in[i].gl_Position;
        vColor = blue[i];
        EmitVertex();
        
        gl_Position = model * vec4(gl_in[i].gl_Position.xyz + normal[i] * 0.3, 1.0);
        vColor = blue[i];
        EmitVertex();
        EndPrimitive();
    }
}