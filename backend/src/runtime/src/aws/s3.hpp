#ifndef AWS_S3_HPP
#define AWS_S3_HPP

#include <string>

#include <aws/s3/S3Client.h>
#include <aws/s3/S3Errors.h>

namespace runtime {
namespace aws {
namespace s3 {

Aws::S3::S3Client* MakeClient();
std::string LookupErrorString(const Aws::S3::S3Errors& code);

}  // namespace s3
}  // namespace aws
}  // namespace runtime

#endif  // AWS_S3_HPP
