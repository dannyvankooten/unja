# List of titles 
- {{ title }}
- {{title}}
- {{ title}}
- {{title }}

# Dot notation
{{ home.title }}

# For block
{% for p in posts %}
    - {{ p.title }}

    (
    {% for t in p.tags %}
        {{ t }},
    {% endfor %}
    )
{% endfor %}

