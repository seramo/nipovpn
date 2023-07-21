#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <vector>
#include <asio.hpp>

#include "header.hpp"

const std::string statusStringOk = "HTTP/1.0 200 OK\r\n";
const std::string statusStringBadRequest = "HTTP/1.0 400 Bad Request\r\n";
const std::string statusStringUnauthorized = "HTTP/1.0 401 Unauthorized\r\n";
const std::string statusStringForbidden = "HTTP/1.0 403 Forbidden\r\n";
const std::string statusStringNotFound = "HTTP/1.0 404 Not Found\r\n";
const std::string statusStringInternalServerError = "HTTP/1.0 500 Internal Server Error\r\n";
const std::string statusStringServiceUnavailable = "HTTP/1.0 503 Service Unavailable\r\n";

const char miscNameValueSeparator[] = { ':', ' ' };
const char miscCrlf[] = { '\r', '\n' };

struct mimeMapping {
	const char* extension;
	const char* mimeType;
};

std::string mimeExtensionToType(const std::string& extension);

struct response{
	enum statusType{
		ok = 200,
		badRequest = 400,
		unauthorized = 401,
		forbidden = 403,
		notFound = 404,
		internalServerError = 500,
		serviceUnavailable = 503
	} status;

	std::vector<header> headers;
	body responseBody;
	std::vector<asio::const_buffer> toBuffers();
	static response stockResponse(statusType status);
};

std::string statusToString(response::statusType status);
asio::const_buffer statusToBuffer(response::statusType status);

#endif //RESPONSE_HPP