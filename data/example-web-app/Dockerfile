FROM python:3.13-alpine

RUN addgroup -S webgroup && adduser -S webuser -G webgroup

COPY ./src /app

WORKDIR /app

USER webuser

RUN pip install --no-cache-dir --upgrade pip \
    && pip install --no-cache-dir -r requirements.txt

# Ensure /home/webuser/.local/bin is in PATH (dir of gunicorn binary)
ENV PATH="/home/webuser/.local/bin:$PATH"

EXPOSE 8080/tcp

ENTRYPOINT ["gunicorn"]
CMD ["--bind", "0.0.0.0:8080", "app:app"]
