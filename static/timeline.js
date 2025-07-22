// Timeline page JavaScript - Basic UI interactions (no backend functionality)

document.addEventListener("DOMContentLoaded", function () {
  // Fetch and render posts from backend using dynamic timeline
  function loadPosts() {
    fetch('/api/timeline')
      .then(response => response.json())
      .then(posts => {
        const postsFeed = document.querySelector('.posts-feed');
        postsFeed.innerHTML = '';
        posts.forEach(post => {
          const newPost = document.createElement('article');
          newPost.className = 'post';
          newPost.innerHTML = `
            <div class="post-header">
              <div class="post-user-info">
                <h4>${post.user_name}</h4>
              </div>
              <div class="post-options">
                <i class="fas fa-ellipsis-h"></i>
              </div>
            </div>
            <div class="post-content">
              <p>${post.content}</p>
            </div>
            <div class="post-stats">
              <span class="likes-count">👍 ${post.like_count || 0} likes</span>
              <span class="comments-count">💬 ${post.comment_count || 0} comments</span>
            </div>
            <div class="post-actions">
              <button class="action-btn like-btn">
                <i class="far fa-thumbs-up"></i> Like
              </button>
              <button class="action-btn comment-btn">
                <i class="far fa-comment"></i> Comment
              </button>
            </div>
            <div class="comments-section" style="display:none;">
              <div class="write-comment">
                <input type="text" placeholder="Write a comment..." class="comment-input">
                <button class="send-comment-btn">
                  <i class="fas fa-paper-plane"></i>
                </button>
              </div>
              <div class="comments-list"></div>
            </div>
          `;
          newPost.setAttribute('data-post-id', post.id); // Set data-post-id
          postsFeed.appendChild(newPost);
          attachPostEventListeners(newPost);
        });
      });
  }

  // Helper to get username from sessionStorage or cookie
  function getCurrentUsername() {
    let username = sessionStorage.getItem('username');
    if (username) return username;
    const match = document.cookie.match(/username=([^;]+)/);
    if (match) return match[1];
    return '';
  }

  // Fetch and render comments for a post
  function loadCommentsForPost(postId, commentsListElem) {
    fetch(`/api/comments?post_id=${postId}`)
      .then(r => r.json())
      .then(comments => {
        commentsListElem.innerHTML = '';
        comments.forEach(comment => {
          const newComment = document.createElement('div');
          newComment.className = 'comment';
          let commentTime = '';
          if (typeof comment.created_at === 'number') {
            commentTime = new Date(comment.created_at * 1000).toLocaleString('en-US', { timeZone: 'Africa/Cairo' });
          } else if (typeof comment.created_at === 'string') {
            // Always replace space with T for ISO compatibility
            const d = new Date(comment.created_at.replace(' ', 'T'));
            commentTime = isNaN(d) ? comment.created_at : d.toLocaleString('en-US', { timeZone: 'Africa/Cairo' });
          }
          newComment.innerHTML = `
            <div class="comment-content">
              <div class="comment-bubble">
                <strong>${comment.user_name}</strong>
                <p>${comment.content}</p>
              </div>
              <div class="comment-actions">
                <button class="comment-like">Like</button>
                <button class="comment-reply">Reply</button>
              </div>
            </div>
          `;
          commentsListElem.appendChild(newComment);
        });
      });
  }

  // Create post functionality (send to backend)
  const createPostInput = document.querySelector(".create-post-input");
  const postButton = document.querySelector(".post-btn");

  if (postButton) {
    postButton.addEventListener("click", function () {
      const postContent = createPostInput.value.trim();
      const userName = getCurrentUsername();
      if (!userName) {
        alert('No username found. Please log in again.');
        return;
      }
      if (postContent) {
        fetch('/api/posts', {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json'
          },
          body: JSON.stringify({
            content: postContent
          })
        })
        .then(response => {
          if (response.ok) {
            createPostInput.value = '';
            loadPosts(); // Reload posts after successful creation
          } else {
            response.text().then(text => alert('Failed to create post: ' + text));
          }
        })
        .catch(error => {
          console.error('Error creating post:', error);
          alert('Error creating post');
        });
      }
    });
  }

  // Attach event listeners to post actions (like, comment, etc.)
  function attachPostEventListeners(post) {
    const likeBtn = post.querySelector('.like-btn');
    const commentBtn = post.querySelector('.comment-btn');
    const sendCommentBtn = post.querySelector('.send-comment-btn');
    const commentInput = post.querySelector('.comment-input');
    const commentsSection = post.querySelector('.comments-section');
    const commentsList = post.querySelector('.comments-list');
    const postId = post.getAttribute('data-post-id');

    // Load comments from backend when post is rendered
    if (postId && commentsList) {
      loadCommentsForPost(postId, commentsList);
    }
    
    // Load like status from backend when post is rendered
    if (postId && likeBtn) {
      fetch(`/api/likes/status?post_id=${postId}`)
        .then(response => response.json())
        .then(data => {
          if (data.liked) {
            likeBtn.classList.add('liked');
            likeBtn.innerHTML = '<i class="fas fa-thumbs-up"></i> Liked';
          } else {
            likeBtn.classList.remove('liked');
            likeBtn.innerHTML = '<i class="far fa-thumbs-up"></i> Like';
          }
        })
        .catch(error => {
          console.error('Error loading like status:', error);
        });
    }

    likeBtn.addEventListener('click', function () {
      const button = this;
      const postId = post.getAttribute('data-post-id');
      
      if (postId) {
        // Send like toggle request to backend
        fetch('/api/likes/toggle', {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json'
          },
          body: JSON.stringify({
            post_id: parseInt(postId)
          })
        })
        .then(response => response.json())
        .then(data => {
          if (data.success) {
            // Update button appearance based on like status
            if (data.liked) {
              button.classList.add('liked');
              button.innerHTML = '<i class="fas fa-thumbs-up"></i> Liked';
            } else {
              button.classList.remove('liked');
              button.innerHTML = '<i class="far fa-thumbs-up"></i> Like';
            }
            
            // Update like count in post stats
            const likesCountSpan = post.querySelector('.likes-count');
            if (likesCountSpan) {
              const currentCount = parseInt(likesCountSpan.textContent.match(/\d+/)?.[0] || '0');
              const newCount = data.liked ? currentCount + 1 : currentCount - 1;
              likesCountSpan.textContent = `👍 ${Math.max(0, newCount)} likes`;
            }
          } else {
            console.error('Failed to toggle like:', data.message);
          }
        })
        .catch(error => {
          console.error('Error toggling like:', error);
        });
      }
    });

    commentBtn.addEventListener('click', function () {
      if (commentsSection.style.display === 'none' || commentsSection.style.display === '') {
        commentsSection.style.display = 'block';
      } else {
        commentsSection.style.display = 'none';
      }
      commentInput.focus();
    });

    sendCommentBtn.addEventListener('click', function () {
      const commentText = commentInput.value.trim();
      if (commentText && postId) {
        fetch('/api/comments', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({
            post_id: Number(postId),
            content: commentText
          })
        })
        .then(r => {
          if (r.ok) {
            commentInput.value = '';
            loadCommentsForPost(postId, commentsList);
          } else {
            r.text().then(msg => alert('Failed to add comment: ' + msg));
          }
        });
      }
    });

    commentInput.addEventListener('keypress', function (e) {
      if (e.key === 'Enter') {
        sendCommentBtn.click();
      }
    });
  }

  // Initial load
  loadPosts();
  
  // Search functionality
  const searchInput = document.querySelector('.search-box input');
  if (searchInput) {
    let searchTimeout;
    searchInput.addEventListener('input', function() {
      const query = this.value.trim();
      clearTimeout(searchTimeout);
      
      if (query.length > 2) {
        searchTimeout = setTimeout(() => {
          performSearch(query);
        }, 500); // Debounce search
      } else if (query.length === 0) {
        // Clear search results and reload posts
        loadPosts();
      }
    });
  }
  
  // Enhanced search function for users and content
  function performSearch(query) {
    // Search both users and posts (with fallback to client-side post search)
    Promise.all([
      fetch(`/api/friends/search?q=${encodeURIComponent(query)}`).then(r => r.json()),
      fetch(`/api/posts`).then(r => r.json()) // Get all posts for client-side filtering
    ])
    .then(([usersData, allPosts]) => {
      // Filter posts client-side by content and username
      const filteredPosts = allPosts.filter(post => 
        post.content.toLowerCase().includes(query.toLowerCase()) ||
        post.user_name.toLowerCase().includes(query.toLowerCase())
      );
      
      displaySearchResults({
        users: usersData.users || [],
        posts: filteredPosts
      });
    })
    .catch(error => {
      console.error('Search error:', error);
      // Fallback to user search only
      fetch(`/api/friends/search?q=${encodeURIComponent(query)}`)
        .then(response => response.json())
        .then(data => {
          displaySearchResults({
            users: data.users || [],
            posts: []
          });
        })
        .catch(err => console.error('Fallback search error:', err));
    });
  }
  
  // Display search results for both users and posts
  function displaySearchResults(results) {
    const postsFeed = document.querySelector('.posts-feed');
    if (!postsFeed) return;
    
    postsFeed.innerHTML = '';
    
    const { users = [], posts = [] } = results;
    
    if (users.length === 0 && posts.length === 0) {
      postsFeed.innerHTML = '<div class="no-results">No results found</div>';
      return;
    }
    
    const searchResultsContainer = document.createElement('div');
    searchResultsContainer.className = 'search-results-container';
    searchResultsContainer.innerHTML = '<h3>Search Results</h3>';
    
    // Display users section
    if (users.length > 0) {
      const usersSection = document.createElement('div');
      usersSection.className = 'search-section';
      usersSection.innerHTML = '<h4><i class="fas fa-users"></i> Users</h4>';
      
      users.forEach(user => {
        const userCard = document.createElement('div');
        userCard.className = 'user-search-result';
        userCard.innerHTML = `
          <div class="user-info">
            <div class="user-avatar"></div>
            <div class="user-details">
              <h5>${user.username}</h5>
              <p>${user.display_name || user.username}</p>
            </div>
          </div>
          <button class="add-friend-btn" data-username="${user.username}">
            <i class="fas fa-user-plus"></i> Add Friend
          </button>
        `;
        usersSection.appendChild(userCard);
      });
      
      searchResultsContainer.appendChild(usersSection);
    }
    
    // Display posts section
    if (posts.length > 0) {
      const postsSection = document.createElement('div');
      postsSection.className = 'search-section';
      postsSection.innerHTML = '<h4><i class="fas fa-file-text"></i> Posts</h4>';
      
      posts.forEach(post => {
        const postCard = document.createElement('article');
        postCard.className = 'post search-post';
        postCard.innerHTML = `
          <div class="post-header">
            <div class="post-user-info">
              <h5>${post.user_name}</h5>
              <p class="post-time">${new Date(post.timestamp).toLocaleDateString()}</p>
            </div>
          </div>
          <div class="post-content">
            <p>${post.content}</p>
          </div>
          <div class="post-stats">
            <span class="likes-count">👍 ${post.like_count || 0} likes</span>
            <span class="comments-count">💬 ${post.comment_count || 0} comments</span>
          </div>
        `;
        postsSection.appendChild(postCard);
      });
      
      searchResultsContainer.appendChild(postsSection);
    }
    
    postsFeed.appendChild(searchResultsContainer);
    
    // Add event listeners for add friend buttons
    searchResultsContainer.querySelectorAll('.add-friend-btn').forEach(button => {
      button.addEventListener('click', function() {
        const username = this.getAttribute('data-username');
        sendFriendRequest(username, this);
      });
    });
  }
  
  // Send friend request function
  function sendFriendRequest(username, button) {
    fetch('/api/friends/request', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json'
      },
      body: JSON.stringify({
        target_username: username
      })
    })
    .then(response => response.json())
    .then(data => {
      if (data.success) {
        button.innerHTML = '<i class="fas fa-check"></i> Request Sent';
        button.style.background = '#42b883';
        button.disabled = true;
      } else {
        alert(data.message || 'Failed to send friend request');
      }
    })
    .catch(error => {
      console.error('Error sending friend request:', error);
      alert('Failed to send friend request');
    });
  }

  // Like and comment button interactions are now handled in attachPostEventListeners function

  // Comment functionality is now integrated in attachPostEventListeners function

  // Comment event listeners are now handled in attachPostEventListeners function



  // Friend request actions (right sidebar)
  const acceptButtons = document.querySelectorAll(".accept-btn");
  const declineButtons = document.querySelectorAll(".decline-btn");
  const addFriendButtons = document.querySelectorAll(".add-friend-btn");

  acceptButtons.forEach((button) => {
    button.addEventListener("click", function () {
      const requestCard = this.closest(".friend-request");
      requestCard.style.opacity = "0.5";
      this.innerHTML = "Accepted";
      this.disabled = true;

      setTimeout(() => {
        requestCard.remove();
      }, 1000);
    });
  });

  declineButtons.forEach((button) => {
    button.addEventListener("click", function () {
      const requestCard = this.closest(".friend-request");
      requestCard.style.opacity = "0.5";
      this.innerHTML = "Declined";
      this.disabled = true;

      setTimeout(() => {
        requestCard.remove();
      }, 1000);
    });
  });

  addFriendButtons.forEach((button) => {
    button.addEventListener("click", function () {
      this.innerHTML = "Request Sent";
      this.disabled = true;
      this.style.background = "#42b883";
    });
  });

  // Navigation icon hover effects
  const navIcons = document.querySelectorAll(".nav-icon");
  navIcons.forEach((icon) => {
    icon.addEventListener("mouseenter", function () {
      this.style.transform = "scale(1.1)";
    });

    icon.addEventListener("mouseleave", function () {
      this.style.transform = "scale(1)";
    });
  });
  
  // Load current user info on page load
  loadCurrentUser();
});

// Load current user info
function loadCurrentUser() {
  // Get username from session or API call
  fetch('/api/user/current')
    .then(response => response.json())
    .then(data => {
      if (data.username) {
        document.getElementById('current-username').textContent = data.username;
        document.getElementById('sidebar-username').textContent = data.username;
        document.getElementById('sidebar-username-handle').textContent = '@' + data.username;
      }
    })
    .catch(error => {
      console.log('Could not load current user info');
    });
}

// Logout function
function logout() {
  if (confirm('Are you sure you want to logout?')) {
    fetch('/api/logout', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json'
      }
    })
    .then(response => response.json())
    .then(data => {
      if (data.success) {
        showNotification('Logged out successfully', 'success');
        // Redirect to welcome page after short delay
        setTimeout(() => {
          window.location.href = '/welcome';
        }, 1000);
      } else {
        showNotification('Logout failed', 'error');
      }
    })
    .catch(error => {
      console.error('Logout error:', error);
      showNotification('Logout failed', 'error');
    });
  }
}

// Improved notification function
function showNotification(message, type = 'info') {
  // Remove existing notifications
  const existingNotifications = document.querySelectorAll('.notification');
  existingNotifications.forEach(notif => notif.remove());
  
  const notification = document.createElement('div');
  notification.className = `notification notification-${type}`;
  notification.innerHTML = `
    <div class="notification-content">
      <i class="fas ${
        type === 'success' ? 'fa-check-circle' : 
        type === 'error' ? 'fa-exclamation-circle' : 
        'fa-info-circle'
      }"></i>
      <span>${message}</span>
    </div>
  `;
  
  // Add notification styles if not already present
  if (!document.querySelector('#notification-styles')) {
    const style = document.createElement('style');
    style.id = 'notification-styles';
    style.textContent = `
      .notification {
        position: fixed;
        top: 80px;
        right: 20px;
        padding: 16px 20px;
        border-radius: 8px;
        color: white;
        font-weight: 500;
        z-index: 10000;
        animation: slideIn 0.3s ease;
        max-width: 400px;
        box-shadow: 0 4px 12px rgba(0, 0, 0, 0.15);
      }
      .notification-success { background: #42b883; }
      .notification-error { background: #e74c3c; }
      .notification-info { background: #3498db; }
      .notification-content {
        display: flex;
        align-items: center;
        gap: 10px;
      }
      @keyframes slideIn {
        from { transform: translateX(100%); opacity: 0; }
        to { transform: translateX(0); opacity: 1; }
      }
    `;
    document.head.appendChild(style);
  }
  
  document.body.appendChild(notification);
  
  // Auto remove after 3 seconds
  setTimeout(() => {
    notification.style.animation = 'slideIn 0.3s ease reverse';
    setTimeout(() => notification.remove(), 300);
  }, 3000);
}
