#version 400

layout(triangles) in;
layout(line_strip, max_vertices = 18) out;

in vec4 red[];
in vec4 green[];
in vec4 blue[];
in vec3 T[];
in vec3 BT[];
in vec3 N[];

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
        
        gl_Position = model * vec4(gl_in[i].gl_Position.xyz + T[i] * 0.3, 1.0);
        vColor = red[i];
        EmitVertex();
        EndPrimitive();

        //Bin
        gl_Position = model * gl_in[i].gl_Position;
        vColor = green[i];
        EmitVertex();
        
        gl_Position = model * vec4(gl_in[i].gl_Position.xyz + BT[i] * 0.3, 1.0);
        vColor = green[i];
        EmitVertex();
        EndPrimitive();

        //nor
        gl_Position = model * gl_in[i].gl_Position;
        vColor = blue[i];
        EmitVertex();
        
        gl_Position = model * vec4(gl_in[i].gl_Position.xyz + N[i] * 0.3, 1.0);
        vColor = blue[i];
        EmitVertex();
        EndPrimitive();
    }
}