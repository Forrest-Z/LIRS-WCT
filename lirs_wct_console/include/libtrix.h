#ifndef LIBTRIX_H
#define LIBTRIX_H

#include <stdint.h>

/*
 * The mesh name is used as the solid model identifier for
 * ASCII STL output and is written to the header section
 * of binary output. Length constrained by 80 byte header.
 */
#define TRIX_MESH_NAME_MAX 80
#define TRIX_MESH_NAME_DEFAULT "libtrix"

/*
 * The maximum number of faces allowed in a mesh.
 * This value is constrained by the four bytes available
 * to indicate the number of faces in a binary STL file.
 * Technically, no such constraint applies to ASCII STL, but
 * libtrix applies this limit in all cases for consistency.
 * (This value is also given in stdint.h as UINT32_MAX.)
 */
#define TRIX_FACE_MAX 4294967295U

/*
 * Every libtrix function returns a trix_result status code.
 * Any value other than TRIX_OK indicates an error condition;
 * check for success for testing whether result == TRIX_OK.
 */
typedef enum {
	TRIX_OK,
	TRIX_ERR_ARG,
	TRIX_ERR_FILE,
	TRIX_ERR_MEM,
	TRIX_ERR_MAXFACECOUNT
} trix_result;

/*
 * Two STL file formats are in use: human-readable ASCII text
 * and more compact binary. When writing a mesh to file, you
 * must specify which format to use using one of these values.
 */
typedef enum {
	TRIX_STL_BINARY,
	TRIX_STL_ASCII,
	TRIX_STL_DEFAULT = TRIX_STL_BINARY
} trix_stl_mode;

/*
 * When recalculating the normal vectors of faces in a mesh,
 * face orientation is determined from the winding order of
 * the face triangle's vertices. By default, libtrix assumes
 * vertices a, b, and c appear counter-clockwise as seen from
 * "outside" the face (the "right hand rule").
 */
typedef enum {
	TRIX_WINDING_CCW,
	TRIX_WINDING_CW,
	TRIX_WINDING_DEFAULT = TRIX_WINDING_CCW
} trix_winding_order;

/*
 * A trix_vertex is a simple three dimensional vector used
 * to represent triangle vertex coordinates and normal vectors.
 */
typedef struct {
	float x, y, z;
} trix_vertex;

/*
 * A trix_triangle is comprised of three vertices (a, b, and c)
 * as well as a normal vector (n). Applications that generate
 * meshes will typically do so by repeatedly defining a
 * trix_triangle and appending it to the mesh with trixAddTriangle.
 * 
 * The normal vector specifies the orientation of the triangle -
 * it points in the direction the triangle is facing. For many
 * applications, the normal vector may be a null vector (0 0 0)
 * to imply orientation from vertex coordinate winding order.
 * trixUpdateNormals applies this interpretation explicitly.
 */
typedef struct {
	trix_vertex n, a, b, c;
} trix_triangle;

/*
 * Every face in an STL mesh is a triangle. The trix_face
 * structure is used to store an individual trix_triangle in
 * the context of the set of triangles that comprise a mesh.
 * It packages a trix_triangle as a node in a linked list.
 */
struct trix_face_node {
	trix_triangle triangle;
	struct trix_face_node *next;
};
typedef struct trix_face_node trix_face;

/*
 * The trix_mesh is the main libtrix data structure. The mesh
 * is stored as a linked list of faces (no order is implied).
 * Applications are advised not to modify a trix_mesh directly,
 * as the face count and mesh data are maintained by functions.
 */
typedef struct {
	char name[TRIX_MESH_NAME_MAX];
	trix_face *first, *last;
	uint32_t facecount;
} trix_mesh;

/*
 * Functions conforming to the trix_function signature can be
 * invoked with every face in a mesh using the trixApply function.
 * A trix_function is passed two parameters: a pointer to the
 * current face, and an arbitrary application-defined data pointer
 * passed to trixApply (which may be NULL).
 */
typedef trix_result (*trix_function)(trix_face *face, void *data);

/*
 * trixCreate
 * 
 * Allocate a new empty mesh.
 * 
 * new_mesh
 * 	A pointer to a trix_mesh pointer, which will be allocated
 * 	if the function succeeds.
 * name
 * 	A name to associate with the mesh.
 * 	If NULL, the default libtrix mesh name will be used.
 */
trix_result trixCreate(trix_mesh **new_mesh, const char *name);

/*
 * trixRead
 * 
 * Allocate a new mesh read from an STL file.
 * Binary and ASCII STL files are supported.
 * 
 * new_mesh
 * 	A pointer to a trix_mesh pointer, which will be allocated
 * 	if the function succeeds.
 * src_path
 * 	Path to STL file to read.
 * 	If NULL, read from stdin.
 */
trix_result trixRead(trix_mesh **new_mesh, const char *src_path);

/*
 * trixWrite
 * 
 * Write a mesh to an STL file.
 * 
 * mesh
 *  The mesh to write.
 * dst_path
 * 	Path to STL file to write.
 * 	If NULL, write to stdout.
 * mode
 * 	Whether to output ASCII or binary STL format.
 */
trix_result trixWrite(const trix_mesh *mesh, const char *dst_path, trix_stl_mode mode);

/*
 * trixRelease
 * 
 * Free memory allocated for mesh.
 * Mesh no longer usable once released.
 * 
 * mesh
 * 	Pointer to the mesh to release.
 * 	mesh is set to NULL once released.
 */
trix_result trixRelease(trix_mesh **mesh);

/*
 * trixZeroNormals
 * 
 * Reset all face normals in mesh to zero (0 0 0).
 * Triangle vertices are not modified.
 * 
 * mesh
 * 	The mesh whose face normals should be zeroed.
 */
trix_result trixZeroNormals(trix_mesh *mesh);

/*
 * trixUpdateNormals
 * 
 * Recalculate all face normals in mesh.
 * 
 * mesh
 * 	The mesh whose face normals should be updated.
 * order
 * 	Infer face orientation from specified vertex winding order,
 * 	as observed from the "outer" side of each face.
 */
trix_result trixUpdateNormals(trix_mesh *mesh, trix_winding_order order);

/*
 * trixAddTriangle
 * 
 * Append a triangle to the mesh.
 * 
 * mesh
 * 	The mesh to which the triangle should be added.
 * triangle
 * 	A pointer to the triangle to add to the mesh.
 */
trix_result trixAddTriangle(trix_mesh *mesh, const trix_triangle *triangle);

/*
 * trixAddMesh
 * 
 * Add faces from one mesh to another.
 * 
 * mesh
 * 	The mesh to which the faces should be added.
 * src_mesh
 * 	The mesh whose faces should be added.
 * 	src_mesh is not modified.
 */
trix_result trixAddMesh(trix_mesh *mesh, const trix_mesh *src_mesh);

/*
 * trixApply
 * 
 * Apply an arbitrary function to each face in mesh.
 * 
 * mesh
 * 	The mesh to process.
 * func
 * 	A pointer to a function matching the trix_function signature.
 * 	func will be invoked for each face in mesh.
 * 	Each invocation will be passed a pointer to the current face.
 * data
 * 	A pointer to arbitrary data. Passed to each invocation of func.
 * 	May be NULL if not needed.
 */
trix_result trixApply(const trix_mesh *mesh, trix_function func, void *data);

#endif
