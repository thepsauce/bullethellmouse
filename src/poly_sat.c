/*
// SAT algorithm
static int32_t
min_projection(const struct entity *e, const Vec2i axis) {
	int32_t min = INT32_MAX;
	for(uint16_t i = 0; i < e->n; i++) {
		const int32_t dot = e->vertices[i].x * axis.x + e->vertices[i].y * axis.y;
		if(dot < min)
			min = dot;
	}
	return min;
}

static int32_t
max_projection(const struct entity *e, const Vec2i axis) {
	int32_t max = INT32_MIN;
	for(uint16_t i = 0; i < e->n; i++) {
		const int32_t dot = e->vertices[i].x * axis.x + e->vertices[i].y * axis.y;
		if(dot > max)
			max = dot;
	}
	return max;
}

bool
intersect_polygon_polygon(const struct entity *e1, const struct entity *e2) {
	for(uint16_t i = 0; i < e1->n; i++) {
		Vec2i axis;
		if (i == e1->n - 1) {
			axis.x = e1->vertices[0].y - e1->vertices[i].y;
			axis.y = e1->vertices[i].x - e1->vertices[0].x;
		} else {
			axis.x = e1->vertices[i + 1].y - e1->vertices[i].y;
			axis.y = e1->vertices[i].x - e1->vertices[i + 1].x;
		}
		const int32_t e1_min = min_projection(e1, axis);
		const int32_t e1_max = max_projection(e1, axis);
		const int32_t e2_min = min_projection(e2, axis);
		const int32_t e2_max = max_projection(e2, axis);
		if(e1_max < e2_min || e1_min > e2_max)
			return false;
	}
	for(uint16_t i = 0; i < e2->n; i++) {
		Vec2i axis;
		if (i == e2->n - 1) {
			axis.x = e2->vertices[0].y - e2->vertices[i].y;
			axis.y = e2->vertices[i].x - e2->vertices[0].x;
		} else {
			axis.x = e2->vertices[i + 1].y - e2->vertices[i].y;
			axis.y = e2->vertices[i].x - e2->vertices[i + 1].x;
		}
		const int32_t e1_min = min_projection(e1, axis);
		const int32_t e1_max = max_projection(e1, axis);
		const int32_t e2_min = min_projection(e2, axis);
		const int32_t e2_max = max_projection(e2, axis);
		if(e1_max < e2_min || e1_min > e2_max)
			return false;
	}
	return true;
}*/
