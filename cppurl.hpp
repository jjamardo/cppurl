#ifndef _CPPURL_
#define _CPPURL_

#include <curl/curl.h>
#include <string>

class CppurlInitException : public std::exception
{
	virtual const char* what() const throw()
	{
		return "cppurl: libcurl initialization failed!";
	}
};

class Cppurl
{
	public:
		enum ContentType
		{
			json,
			text
		};

		Cppurl()
		{
			curl_global_init(CURL_GLOBAL_ALL);
			curl = curl_easy_init();
			if(!curl)
			{
				CppurlInitException ex;
				throw ex;
			}
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
			userpwd = "";
		}

		~Cppurl()
		{
			curl_easy_cleanup(curl);
			curl_global_cleanup();
		}

		void setAvoidSSL(bool value)
		{
			if (value)
			{
				curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
				curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
			}
			else
			{
				curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
				curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 1L);
			}
		}

		void setVerbose(bool value)
		{
			if (value)
			{
				curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
			}
			else
			{
				curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
			}
		}

		void setUserAndPwd(std::string value)
		{
			if (value.empty() && !userpwd.empty())
			{
				userpwd = "";
				curl_easy_setopt(curl, CURLOPT_USERPWD, NULL);
			}
			else
			{
				userpwd = value;
				curl_easy_setopt(curl, CURLOPT_USERPWD, userpwd.c_str());
			}
		}

		void setContentType(ContentType ct)
		{
			struct curl_slist *headers=NULL;
			std::string type;
			switch (ct)
			{
				case json:
					type = "Content-Type: application/json";
					break;
				case text:
					type = "Content-Type: application/text";
					break;
			}
			headers = curl_slist_append(headers, type.c_str());
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		}

		int post(std::string url, std::string data, std::string& response)
		{
			return perform(url, data, response);
		}

		int get(std::string url, std::string& response)
		{
			return perform(url, "", response);
		}

	private:
		int perform(std::string url, std::string data, std::string& response)
		{
			CURLcode res;

			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

			if (!data.empty())
			{
				curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
			}

			std::string s;
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

			long http_code = 0;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

			res = curl_easy_perform(curl);

			if(res != CURLE_OK)
			{
				response = curl_easy_strerror(res);
			}
			else
			{
				response = s;
			}

			return http_code;
		}

		static int callback(void *contents, int size, int nmemb, std::string *s)
		{
			int newLength = size*nmemb;
			int oldLength = s->size();
			try
			{
				s->resize(oldLength + newLength);
			}
			catch(std::bad_alloc &e)
			{
				return 0;
			}

			copy((char*)contents, (char*)contents+newLength, s->begin()+oldLength);
			return size*nmemb;
		}

		CURL *curl;
		std::string userpwd;
};

#endif
