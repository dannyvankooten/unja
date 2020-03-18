Unja  [![Build Status](https://img.shields.io/travis/dannyvankooten/unja/master)](https://travis-ci.org/dannyvankooten/unja)
 [![License: MIT](https://img.shields.io/github/license/dannyvankooten/unja)](https://github.com/dannyvankooten/unja/blob/master/LICENSE)
==========

A template engine for C, inspired by Jinja and Liquid. 

**This is nowhere near being stable right now!**

### Example

```html+jinja
{% extends "base.html" %}

{% block title %}Members{% endblock %}

{% block content %}
  <ul>
  {% for user in users %}
    <li><a href="{{ user.url }}">{{ user.username }}</a></li>
  {% endfor %}
  </ul>
{% endblock %}
```

### License

MIT