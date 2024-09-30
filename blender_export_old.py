import bpy
import struct

def export(filepath):
    print(f"Exporting mesh {filepath}")

    obj = bpy.context.object
    if obj.type != "MESH":
        print("Selected object is not a mesh")
        return {"CANCELLED"}
    
    mesh = obj.data
    mesh.calc_loop_triangles()
    
    uv_layer = mesh.uv_layers.active.data if mesh.uv_layers.active else None
    
    # Dictionary to store unique vertex + UV pairs
    vertex_uv_map = {}
    vertex_list = []
    
    vertex_index_map = {}  # Maps loop index to unique vertex list index
    current_index = 0
    
    # Collect unique vertices with UVs
    for loop in mesh.loops:
        vert = mesh.vertices[loop.vertex_index]
        pos = vert.co
        
        uv = None

        if uv_layer:
            uv = (uv_layer[loop.index].uv.x, uv_layer[loop.index].uv.y)
            # uv = uv_layer[loop.index].uv
        else:
            uv = (0.0, 0.0)

        vertex_key = (pos, uv)
        
        # If the vertex/uv pair is not already in the map, add it
        if vertex_key not in vertex_uv_map:
            vertex_uv_map[vertex_key] = current_index
            # vertex_list.append((pos, uv))
            vertex_list[current_index] = (pos, uv)
            current_index += 1
        
        # Map loop index to the vertex/uv pair index
        vertex_index_map[loop.index] = vertex_uv_map[vertex_key]
    
    with open(filepath, "wb") as file:
        vertex_count = len(vertex_list)
        print(f"Vertex count: {vertex_count}")
        
        # I: u32
        file.write(struct.pack("I", vertex_count))  # 4 bytes
        
        # Write unique vertices and UVs
        for pos, uv in vertex_list:
            # Swap y and z
            file.write(struct.pack("fff ff", pos[0], pos[2], pos[1], uv[0], uv[1]))  # 3 floats (pos) + 2 floats (UV)
        
        triangle_count = len(mesh.loop_triangles)
        print(f"Triangle count: {triangle_count}")

        # I: u32
        file.write(struct.pack("I", triangle_count))
        
        # Write each triangle"s reindexed vertex indices
        for triangle in mesh.loop_triangles:
            file.write(struct.pack("III", 
                                   vertex_index_map[triangle.loops[0]], 
                                   vertex_index_map[triangle.loops[1]], 
                                   vertex_index_map[triangle.loops[2]]))
    
    print(f"Exported custom mesh data to {filepath}")
    return {"FINISHED"}


export("/home/herret/code/3d_engine/res/meshes/test3.mesh")