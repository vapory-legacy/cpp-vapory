file(GLOB sources "*.cpp" "*.h")

add_library(web3jsonrpc ${sources})

jsonrpcstub_create(web3jsonrpc vap.json
	dev::rpc::VapFace ${CMAKE_CURRENT_SOURCE_DIR} VapFace
	VapClient ${CMAKE_CURRENT_BINARY_DIR} VapClient
)

jsonrpcstub_create(web3jsonrpc debug.json
	dev::rpc::DebugFace ${CMAKE_CURRENT_SOURCE_DIR} DebugFace
	DebugClient ${CMAKE_CURRENT_BINARY_DIR} DebugClient
)

jsonrpcstub_create(web3jsonrpc whisper.json
	dev::rpc::WhisperFace ${CMAKE_CURRENT_SOURCE_DIR} WhisperFace
	WhisperClient ${CMAKE_CURRENT_BINARY_DIR} WhisperClient
)

jsonrpcstub_create(web3jsonrpc net.json
	dev::rpc::NetFace ${CMAKE_CURRENT_SOURCE_DIR} NetFace
	NetClient ${CMAKE_CURRENT_BINARY_DIR} NetClient
)

jsonrpcstub_create(web3jsonrpc web3.json
	dev::rpc::Web3Face ${CMAKE_CURRENT_SOURCE_DIR} Web3Face
	Web3Client ${CMAKE_CURRENT_BINARY_DIR} Web3Client
)

jsonrpcstub_create(web3jsonrpc admin_net.json
	dev::rpc::AdminNetFace ${CMAKE_CURRENT_SOURCE_DIR} AdminNetFace
	AdminNetClient ${CMAKE_CURRENT_BINARY_DIR} AdminNetClient
)

jsonrpcstub_create(web3jsonrpc admin_vap.json
	dev::rpc::AdminVapFace ${CMAKE_CURRENT_SOURCE_DIR} AdminVapFace
	AdminVapClient ${CMAKE_CURRENT_BINARY_DIR} AdminVapClient
)

jsonrpcstub_create(web3jsonrpc admin_utils.json
	dev::rpc::AdminUtilsFace ${CMAKE_CURRENT_SOURCE_DIR} AdminUtilsFace
	AdminUtilsClient ${CMAKE_CURRENT_BINARY_DIR} AdminUtilsClient
)

jsonrpcstub_create(web3jsonrpc personal.json
	dev::rpc::PersonalFace ${CMAKE_CURRENT_SOURCE_DIR} PersonalFace
	PersonalClient ${CMAKE_CURRENT_BINARY_DIR} PersonalClient
)

jsonrpcstub_create(web3jsonrpc test.json
	dev::rpc::TestFace ${CMAKE_CURRENT_SOURCE_DIR} TestFace
	TestClient ${CMAKE_CURRENT_BINARY_DIR} TestClient
)

target_include_directories(web3jsonrpc PRIVATE ../utils)

target_link_libraries(web3jsonrpc vapashseal webthree whisper JsonRpcCpp::Server)
