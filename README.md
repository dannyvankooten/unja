Unja
=====

Unja is (an attempt at) a template engine for C, inspired by Jinja and Liquid.

This is nowhere near being stable right now.

### Example

```
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