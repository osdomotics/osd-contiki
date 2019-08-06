all: CA.c server.example.com.c client.example.com.c

CA.crt CA.key:
	./genca

client.example.com.crt client.example.com.key: CA.key
	./genclientcert client.example.com

server.example.com.crt server.example.com.key: CA.key
	./genservercert server.example.com

%.der: %.crt
	openssl x509 -in $< -outform der -out $@

%.kder: %.key
	openssl pkcs8 -topk8 -inform pem -outform der -in $< -out $@ -nocrypt

CA.c: CA.crt
	./genc $< > $@

%.c: %.der %.kder
	./genc $^ > $@

