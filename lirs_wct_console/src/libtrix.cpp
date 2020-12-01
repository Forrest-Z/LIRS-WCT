#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "libtrix.h"

trix_result trixApply(const trix_mesh *mesh, trix_function func, void *data) {
	trix_face *face, *next;
	trix_result rr;
	
	if (mesh == NULL) {
		return TRIX_ERR_ARG;
	}
	
	face = mesh->first;
	while (face != NULL) {
		next = face->next;
		
		if ((rr = (func)(face, data)) != TRIX_OK) {
			return rr;
		}
		
		face = next;
	}
	
	return TRIX_OK;
}

static trix_result trixWriteHeaderBinary(FILE *stl_dst, const trix_mesh *mesh) {
	char header[80];
	
	if (mesh == NULL) {
		return TRIX_ERR_ARG;
	}
	
	// for now, just writing mesh name to header.
	// should check that name doesn't begin with "solid"
	strncpy(header, mesh->name, 80);
	header[79] = '\0';
	
	if (fwrite(header, 80, 1, stl_dst) != 1) {
		return TRIX_ERR_FILE;
	}
	
	if (fwrite(&mesh->facecount, 4, 1, stl_dst) != 1) {
		return TRIX_ERR_FILE;
	}
	
	return TRIX_OK;
}

static trix_result trixWriteHeaderASCII(FILE *stl_dst, const trix_mesh *mesh) {
	
	if (mesh == NULL) {
		return TRIX_ERR_ARG;
	}
	
	if (fprintf(stl_dst, "solid %s\n", mesh->name) < 0) {
		return TRIX_ERR_FILE;
	}
	
	return TRIX_OK;
}

static trix_result trixWriteHeader(FILE *stl_dst, const trix_mesh *mesh, trix_stl_mode mode) {
	return (mode == TRIX_STL_ASCII
			? trixWriteHeaderASCII(stl_dst, mesh)
			: trixWriteHeaderBinary(stl_dst, mesh));
}

static trix_result trixWriteFooterASCII(FILE *stl_dst, const trix_mesh *mesh) {
	
	if (mesh == NULL) {
		return TRIX_ERR_ARG;
	}
	
	if (fprintf(stl_dst, "endsolid %s\n", mesh->name) < 0) {
		return TRIX_ERR_FILE;
	}
	
	return TRIX_OK;
}

static trix_result trixWriteFooter(FILE *stl_dst, const trix_mesh *mesh, trix_stl_mode mode) {
	if (mode == TRIX_STL_ASCII) {
		return trixWriteFooterASCII(stl_dst, mesh);
	}
	// no footer in binary mode
	return TRIX_OK;
}

static trix_result trixWriteFaceASCII(FILE *stl_dst, trix_face *face) {
	
	if (face == NULL) {
		return TRIX_ERR_ARG;
	}
	
	if (fprintf(stl_dst,
			"facet normal %f %f %f\n"
			"outer loop\n"
			"vertex %f %f %f\n"
			"vertex %f %f %f\n"
			"vertex %f %f %f\n"
			"endloop\n"
			"endfacet\n",
			face->triangle.n.x, face->triangle.n.y, face->triangle.n.z,
			face->triangle.a.x, face->triangle.a.y, face->triangle.a.z,
			face->triangle.b.x, face->triangle.b.y, face->triangle.b.z,
			face->triangle.c.x, face->triangle.c.y, face->triangle.c.z) < 0) {
		return TRIX_ERR_FILE;
	}
	
	return TRIX_OK;
}

static trix_result trixWriteFaceBinary(FILE *stl_dst, trix_face *face) {
	unsigned short attributes = 0;
	
	// triangle struct is 12 floats in sequence needed for output!
	if (fwrite(&face->triangle, 4, 12, stl_dst) != 12) {
		return TRIX_ERR_FILE;
	}
	
	if (fwrite(&attributes, 2, 1, stl_dst) != 1) {
		return TRIX_ERR_FILE;
	}
	
	return TRIX_OK;
}

static trix_result trixWriteFace(FILE *stl_dst, trix_face *face, trix_stl_mode mode) {
	return (mode == TRIX_STL_ASCII
			? trixWriteFaceASCII(stl_dst, face)
			: trixWriteFaceBinary(stl_dst, face));
}

static void trixCloseOutput(FILE *dst) {
	if (dst != NULL && dst != stdout) {
		fclose(dst);
	}
}

// stl_dst is assumed to be open and ready for writing
trix_result trixWrite(const trix_mesh *mesh, const char *dst_path, trix_stl_mode mode) {
	trix_face *face;
	FILE *stl_dst;
	trix_result rr;
	
	if (mesh == NULL) {
		return TRIX_ERR_ARG;
	}
	
	if (dst_path == NULL) {
		stl_dst = stdout;
	} else if ((stl_dst = fopen(dst_path, "w")) == NULL) {
		return TRIX_ERR_FILE;
	}
	
	if ((rr = trixWriteHeader(stl_dst, mesh, mode)) != TRIX_OK) {
		trixCloseOutput(stl_dst);
		return rr;
	}
	
	face = mesh->first;
	while (face != NULL) {
		
		if ((rr = trixWriteFace(stl_dst, face, mode)) != TRIX_OK) {
			trixCloseOutput(stl_dst);
			return rr;
		}
		
		face = face->next;
	}
	
	if ((rr = trixWriteFooter(stl_dst, mesh, mode)) != TRIX_OK) {
		trixCloseOutput(stl_dst);
		return rr;
	}
	
	trixCloseOutput(stl_dst);
	return TRIX_OK;
}

static trix_result trixReadBinary(FILE *stl_src, trix_mesh **dst_mesh) {
	
	uint32_t facecount, f;
	uint16_t attribute;
	
	trix_triangle triangle;
	trix_mesh *mesh;
	trix_result rr;
	
	// fread instead to get header
	if (fseek(stl_src, 80, SEEK_SET) != 0) {
		return TRIX_ERR_FILE;
	}
	
	if (fread(&facecount, 4, 1, stl_src) != 1) {
		return TRIX_ERR_FILE;
	}
	
	if ((rr = trixCreate(&mesh, NULL)) != TRIX_OK) {
		return rr;
	}
	
	// consider succeeding as long as we can read a whole number of faces,
	// even if that number read does not match facecount (only optionally treat it as an error?) 
	
	for (f = 0; f < facecount; f++) {
		
		if (fread(&triangle, 4, 12, stl_src) != 12) {
			(void)trixRelease(&mesh);
			return TRIX_ERR_FILE;
		}
		
		if (fread(&attribute, 2, 1, stl_src) != 1) {
			(void)trixRelease(&mesh);
			return TRIX_ERR_FILE;
		}
		
		if ((rr = trixAddTriangle(mesh, &triangle)) != TRIX_OK) {
			(void)trixRelease(&mesh);
			return rr;
		}
	}
	
	*dst_mesh = mesh;
	return TRIX_OK;
}

static trix_result trixReadTriangleASCII(FILE *stl_src, trix_triangle *triangle) {
	if (fscanf(stl_src,
			" facet normal %20f %20f %20f"
			" outer loop"
			" vertex %20f %20f %20f"
			" vertex %20f %20f %20f"
			" vertex %20f %20f %20f"
			" endloop"
			" endfacet ",
			&triangle->n.x, &triangle->n.y, &triangle->n.z,
			&triangle->a.x, &triangle->a.y, &triangle->a.z,
			&triangle->b.x, &triangle->b.y, &triangle->b.z,
			&triangle->c.x, &triangle->c.y, &triangle->c.z) != 12) {
		return TRIX_ERR_FILE;
	}
	
	return TRIX_OK;
}

static trix_result trixReadASCII(FILE *stl_src, trix_mesh **dst_mesh) {
	trix_mesh *mesh;
	trix_triangle triangle;
	trix_result rr;
	char linebuffer[256];
	char namebuffer[128];
	
	// read first line
	if (fgets(linebuffer, 256, stl_src) == NULL) {
		return TRIX_ERR_FILE;
	}
	
	// check for 'solid' keyword and identifier (ignore any other comments)
	if (sscanf(linebuffer, " solid %127s ", namebuffer) != 1) {
		return TRIX_ERR_FILE;
	}
		
	if ((rr = trixCreate(&mesh, NULL)) != TRIX_OK) {
		return rr;
	}
	
	while (trixReadTriangleASCII(stl_src, &triangle) == TRIX_OK) {
		if ((rr = trixAddTriangle(mesh, &triangle)) != TRIX_OK) {
			(void)trixRelease(&mesh);
			return rr;
		}
	}
	
	// ignore endsolid footer; to be pedantic, check that it
	// specifies the same identifier given with solid header
		
	*dst_mesh = mesh;
	return TRIX_OK;
}


trix_result trixRead(trix_mesh **new_mesh, const char *src_path) {
	trix_mesh *mesh;
	FILE *stl_src;
	trix_result rr;
		
	if (src_path == NULL) {
		stl_src = stdin;
	} else if ((stl_src = fopen(src_path, "r")) == NULL) {
		return TRIX_ERR_FILE;
	}
	
	// first try to read the file as ASCII STL,
	// since we can easily check for 'solid' keyword in first line
	rr = trixReadASCII(stl_src, &mesh);
	if (rr == TRIX_ERR_FILE) {
		
		// we've already opened the file, so a file error just
		// means it can't be parsed as ASCII; retry as binary
		rewind(stl_src);
		rr = trixReadBinary(stl_src, &mesh);
	}
	
	if (stl_src != stdin) {
		fclose(stl_src);
	}
	
	if (rr == TRIX_OK) {
		*new_mesh = mesh;
	}
	
	return rr;
}

trix_result trixAddTriangle(trix_mesh *mesh, const trix_triangle *triangle) {
	trix_face *face;
	
	if (mesh == NULL) {
		return TRIX_ERR_ARG;
	}
	
	// disallow adding more faces than we can count
	// (constrained by the 4 bytes available for facecount in binary STL)
	if (mesh->facecount == TRIX_FACE_MAX) {
		return TRIX_ERR_MAXFACECOUNT;
	}
	
	if ((face = (trix_face *)malloc(sizeof(trix_face))) == NULL) {
		return TRIX_ERR_MEM;
	}
	
	face->triangle = *triangle;
	face->next = NULL;
		
	if (mesh->last == NULL) {
		// this is the first face
		mesh->first = face;
		mesh->last = face;
	} else {
		mesh->last->next = face;
		mesh->last = face;
	}
	
	mesh->facecount += 1;
	return TRIX_OK;
}

static trix_result trixAddFaceFunc(trix_face *face, trix_mesh *dst_mesh) {
	return trixAddTriangle(dst_mesh, &face->triangle);
}

trix_result trixAddMesh(trix_mesh *dst_mesh, const trix_mesh *src_mesh) {
	return trixApply(src_mesh, (trix_function)trixAddFaceFunc, (void *)dst_mesh);
}

trix_result trixCreate(trix_mesh **new_mesh, const char *name) {
	trix_mesh *mesh;
	
	if ((mesh = (trix_mesh *)malloc(sizeof(trix_mesh))) == NULL) {
		return TRIX_ERR_MEM;
	}
	
	if (name == NULL) {
		strncpy(mesh->name, TRIX_MESH_NAME_DEFAULT, TRIX_MESH_NAME_MAX);
	} else {
		strncpy(mesh->name, name, TRIX_MESH_NAME_MAX);
		mesh->name[TRIX_MESH_NAME_MAX - 1] = '\0';
	}
	
	mesh->first = NULL;
	mesh->last = NULL;
	mesh->facecount = 0;
	
	*new_mesh = mesh;
	return TRIX_OK;
}

static trix_result trixZeroFaceNormal(trix_face *face) {
	if (face == NULL) {
		return TRIX_ERR_ARG;
	}
	
	face->triangle.n.x = 0;
	face->triangle.n.y = 0;
	face->triangle.n.z = 0;
	
	return TRIX_OK;
}

trix_result trixZeroNormals(trix_mesh *mesh) {
	return trixApply(mesh, (trix_function)trixZeroFaceNormal, NULL);
}

// sets result to difference of vectors a and b (b - a)
static void vector_difference(const trix_vertex *a, const trix_vertex *b, trix_vertex *result) {
	result->x = b->x - a->x;
	result->y = b->y - a->y;
	result->z = b->z - a->z;
}

// sets result to cross product of vectors a and b (a x b)
// https://en.wikipedia.org/wiki/Cross_product#Mnemonic
static void vector_crossproduct(const trix_vertex *a, const trix_vertex *b, trix_vertex *result) {
	result->x = a->y * b->z - a->z * b->y;
	result->y = a->z * b->x - a->x * b->z;
	result->z = a->x * b->y - a->y * b->x;
}

// sets result to unit vector codirectional with vector v (v / ||v||)
static void vector_unitvector(const trix_vertex *v, trix_vertex *result) {
	float mag = sqrtf((v->x * v->x) + (v->y * v->y) + (v->z * v->z));
	result->x = v->x / mag;
	result->y = v->y / mag;
	result->z = v->z / mag;
}

static trix_result trixUpdateFaceNormal(trix_face *face, trix_winding_order *order) {
	trix_vertex u, v, cp, n;
	
	if (face == NULL) {
		return TRIX_ERR_ARG;
	}
	
	// vectors u and v are triangle sides ab and bc
	vector_difference(&face->triangle.a, &face->triangle.b, &u);
	vector_difference(&face->triangle.b, &face->triangle.c, &v);
	
	// the cross product of two vectors is perpendicular to both
	// since vectors u and v both lie in the plane of triangle abc,
	// the cross product is perpendicular to the triangle's surface
	if (order == NULL || *order == TRIX_WINDING_CCW) {
		vector_crossproduct(&u, &v, &cp);
	} else {
		vector_crossproduct(&v, &u, &cp);
	}
	
	// normalize the cross product to unit length to get surface normal n
	vector_unitvector(&cp, &n);
	
	face->triangle.n.x = n.x;
	face->triangle.n.y = n.y;
	face->triangle.n.z = n.z;
	return TRIX_OK;
}

trix_result trixUpdateNormals(trix_mesh *mesh, trix_winding_order order) {
	return trixApply(mesh, (trix_function)trixUpdateFaceNormal, (void *)&order);
}

trix_result trixRelease(trix_mesh **mesh) {
	trix_face *face, *nextface;
	
	if (mesh == NULL || *mesh == NULL) {
		return TRIX_ERR_ARG;
	}
	
	face = (*mesh)->first;
	while (face != NULL) {
		nextface = face->next;
		free(face);
		face = nextface;
	}
	
	free(*mesh);
	*mesh = NULL;
	return TRIX_OK;
}
