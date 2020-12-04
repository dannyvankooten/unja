Unja  [![Build Status](https://img.shields.io/travis/dannyvankooten/unja/master)](https://travis-ci.org/dannyvankooten/unja)
 [![License: MIT](https://img.shields.io/github/license/dannyvankooten/unja)](https://github.com/dannyvankooten/unja/blob/master/LICENSE)
==========

A template engine for C, inspired by Jinja and Liquid. 

### Example

File `base.tmpl`:
```html+jinja
<html>
	<head><title>{% block title %}Default title{% endblock %}</title></head>
	<body>
		{% block content %}
		{% endblock %}
	</body>
</html>
```

File `child.tmpl`:
```html+jinja
{% extends "base.html" %}

{% block title %}Users{% endblock %}

{% block content %}
	<ul>
	{% for user in users %}
		<li><a href="{{ user.url }}">{{ user.username | lower }}</a></li>
	{% endfor %}
	</ul>
{% endblock %}
```

File: `example.c`
```c
int main() {
	// parse all templates in the given directory
	struct env *env = env_new("./");

	// create a hashmap for storing template variables
	struct hashmap *vars = hashmap_new();
	hashmap_insert(vars, "name", "John Doe");

	// execute the template
	char *output = template(env, "child.tmpl", vars);
	printf("%s", output);

	// clean-up allocated memory
	free(output);
	hashmap_free(vars);
	env_free(env);
}
```

### License

MIT