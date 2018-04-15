#include <stdio.h>
#include "render.h"

Sphere scene[] = {
	{ // center C
		.r = 1.f,
		.p = {0.f, 0.f, 0.f},
		.c.ambi = {.05f, .05f, .05f},
		.c.diff = {.5f, .5f, .5f},
		.c.spec = {.6f, .6f, .6f},
	},
	{ // upper O
		.r = .8f,
		.p = {0.f, 1.f, 0.f},
		.c.ambi = {.02f, .05f, .1f},
		.c.diff = {.2f, .5f, 1.f},
		.c.spec = {.6f, .6f, .6f},
	},
	{ // right O
		.r = .8f,
		.p = {.8f, -.6f, 0.f},
		.c.ambi = {.02f, .05f, .1f},
		.c.diff = {.2f, .5f, 1.f},
		.c.spec = {.6f, .6f, .6f},
	},
	{ // upper O left H
		.r = .5f,
		.p = {-.6f, 1.45f, 0.f},
		.c.ambi = {.1f, .09f, .08f},
		.c.diff = {1.f, .9f, .8f},
		.c.spec = {.6f, .6f, .6f},
	},
	{ // left H
		.r = .5f,
		.p = {-.8f, -.6f, 0.f},
		.c.ambi = {.1f, .09f, .08f},
		.c.diff = {1.f, .9f, .8f},
		.c.spec = {.6f, .6f, .6f},
	},
};

