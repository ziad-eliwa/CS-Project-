// Friends page JavaScript - Full backend integration

document.addEventListener("DOMContentLoaded", function () {
  // Load initial data from backend
  loadFriends();
  loadFriendRequests();
  loadFriendSuggestions();
  loadCurrentUser();
  
  // Load friends from backend
  function loadFriends() {
    fetch('/api/friends')
      .then(response => response.json())
      .then(data => {
        const friendsContainer = document.querySelector('.friends-list');
        if (friendsContainer && data.friends) {
          friendsContainer.innerHTML = '';
          data.friends.forEach(friend => {
            const friendCard = createFriendCard(friend);
            friendsContainer.appendChild(friendCard);
          });
        }
      })
      .catch(error => {
        console.error('Error loading friends:', error);
        showNotification('Failed to load friends', 'error');
      });
  }
  
  // Load friend suggestions from backend
  function loadFriendSuggestions() {
    fetch('/api/friends/suggestions')
      .then(response => response.json())
      .then(data => {
        const suggestionsContainer = document.querySelector('.suggestions-list');
        if (suggestionsContainer && data.suggestions) {
          suggestionsContainer.innerHTML = '';
          data.suggestions.forEach(suggestion => {
            const suggestionCard = createSuggestionCard(suggestion);
            suggestionsContainer.appendChild(suggestionCard);
          });
        }
      })
      .catch(error => {
        console.error('Error loading suggestions:', error);
        showNotification('Failed to load friend suggestions', 'error');
      });
  }
  
  // Search users function
  function searchUsers(query) {
    if (!query.trim()) return;
    
    fetch(`/api/friends/search?q=${encodeURIComponent(query)}`)
      .then(response => response.json())
      .then(data => {
        const searchResultsContainer = document.querySelector('.search-results');
        if (searchResultsContainer && data.users) {
          searchResultsContainer.innerHTML = '';
          data.users.forEach(user => {
            const userCard = createSearchResultCard(user);
            searchResultsContainer.appendChild(userCard);
          });
        }
      })
      .catch(error => {
        console.error('Error searching users:', error);
        showNotification('Search failed', 'error');
      });
  }
  
  // Create friend card HTML
  function createFriendCard(friend) {
    const card = document.createElement('div');
    card.className = 'friend-card';
    card.innerHTML = `
      <div class="friend-avatar">
        <div class="avatar-placeholder"></div>
        <div class="online-status ${friend.is_online ? 'online' : ''}"></div>
      </div>
      <div class="friend-info">
        <h3>${friend.username}</h3>
        <p>${friend.display_name || friend.username}</p>
      </div>
      <div class="friend-actions">
        <button class="action-btn message-btn">
          <i class="fas fa-comment"></i>
        </button>
        <button class="action-btn remove-friend-btn" data-username="${friend.username}">
          <i class="fas fa-user-minus"></i>
        </button>
      </div>
    `;
    return card;
  }
  
  // Create suggestion card HTML
  function createSuggestionCard(suggestion) {
    const card = document.createElement('div');
    card.className = 'suggestion-card';
    
    // Format mutual friends count with better text
    const mutualCount = suggestion.mutual_friends || 0;
    const mutualText = mutualCount === 0 ? 'No mutual friends' : 
                      mutualCount === 1 ? '1 mutual friend' : 
                      `${mutualCount} mutual friends`;
    
    card.innerHTML = `
      <div class="suggestion-avatar">
        <div class="avatar-placeholder">
          <i class="fas fa-user"></i>
        </div>
      </div>
      <div class="suggestion-info">
        <h4>${suggestion.username}</h4>
        <p class="suggestion-subtitle">${suggestion.display_name || suggestion.username}</p>
        <span class="mutual-friends">
          <i class="fas fa-users"></i> ${mutualText}
        </span>
      </div>
      <div class="suggestion-actions">
        <button class="add-friend-btn" data-username="${suggestion.username}" title="Send friend request">
          <i class="fas fa-user-plus"></i> Add Friend
        </button>
        <button class="remove-suggestion-btn" title="Remove suggestion">
          <i class="fas fa-times"></i>
        </button>
      </div>
    `;
    return card;
  }
  
  // Create search result card HTML
  function createSearchResultCard(user) {
    const card = document.createElement('div');
    card.className = 'search-result-card';
    card.innerHTML = `
      <div class="result-avatar">
        <div class="avatar-placeholder"></div>
      </div>
      <div class="result-info">
        <h5>${user.username}</h5>
        <p>${user.display_name || user.username}</p>
      </div>
      <div class="result-actions">
        <button class="add-friend-result-btn" data-username="${user.username}">
          <i class="fas fa-user-plus"></i> Add Friend
        </button>
      </div>
    `;
    return card;
  }
  
  // Load friend requests from backend
  function loadFriendRequests() {
    fetch('/api/friends/requests')
      .then(response => response.json())
      .then(data => {
        const requestsContainer = document.querySelector('.requests-list');
        if (requestsContainer && data.requests) {
          requestsContainer.innerHTML = '';
          if (data.requests.length === 0) {
            requestsContainer.innerHTML = '<p class="no-requests">No pending friend requests</p>';
          } else {
            data.requests.forEach(request => {
              const requestCard = createFriendRequestCard(request);
              requestsContainer.appendChild(requestCard);
            });
          }
          // Update the badge count
          updateRequestsBadge(data.count);
        }
      })
      .catch(error => {
        console.error('Error loading friend requests:', error);
        showNotification('Failed to load friend requests', 'error');
      });
  }
  // Tab switching functionality
  const tabButtons = document.querySelectorAll(".tab-btn");
  const tabContents = document.querySelectorAll(".tab-content");

  tabButtons.forEach((button) => {
    button.addEventListener("click", function () {
      const targetTab = this.getAttribute("data-tab");

      // Remove active class from all tabs and contents
      tabButtons.forEach((btn) => btn.classList.remove("active"));
      tabContents.forEach((content) => content.classList.remove("active"));

      // Add active class to clicked tab and corresponding content
      this.classList.add("active");
      document.getElementById(targetTab).classList.add("active");
    });
  });

  // Friends search functionality
  const friendsSearchInput = document.querySelector(".friends-search-input");
  const filterSelect = document.querySelector(".filter-select");

  if (friendsSearchInput) {
    friendsSearchInput.addEventListener("input", function () {
      const searchTerm = this.value.trim();
      if (searchTerm.length > 2) {
        // Search backend when user types more than 2 characters
        searchUsers(searchTerm);
      } else {
        // Local search for existing friends
        const friendCards = document.querySelectorAll(".friend-card");
        friendCards.forEach((card) => {
          const friendName = card.querySelector("h3").textContent.toLowerCase();
          if (friendName.includes(searchTerm.toLowerCase())) {
            card.style.display = "block";
          } else {
            card.style.display = "none";
          }
        });
      }
    });
  }
  
  // Add event delegation for dynamically created buttons
  document.addEventListener('click', function(e) {
    // Handle add friend button clicks
    if (e.target.classList.contains('add-friend-btn') || e.target.closest('.add-friend-btn')) {
      const button = e.target.classList.contains('add-friend-btn') ? e.target : e.target.closest('.add-friend-btn');
      const username = button.getAttribute('data-username');
      sendFriendRequest(username, button);
    }
    
    // Handle add friend from search results
    if (e.target.classList.contains('add-friend-result-btn') || e.target.closest('.add-friend-result-btn')) {
      const button = e.target.classList.contains('add-friend-result-btn') ? e.target : e.target.closest('.add-friend-result-btn');
      const username = button.getAttribute('data-username');
      sendFriendRequest(username, button);
    }
    
    // Handle remove friend button clicks
    if (e.target.classList.contains('remove-friend-btn') || e.target.closest('.remove-friend-btn')) {
      const button = e.target.classList.contains('remove-friend-btn') ? e.target : e.target.closest('.remove-friend-btn');
      const username = button.getAttribute('data-username');
      removeFriend(username, button);
    }
    
    // Handle accept friend request
    if (e.target.classList.contains('accept-request-btn') || e.target.closest('.accept-request-btn')) {
      const button = e.target.classList.contains('accept-request-btn') ? e.target : e.target.closest('.accept-request-btn');
      const username = button.getAttribute('data-username');
      respondToFriendRequest(username, 'accept', button);
    }
    
    // Handle decline friend request
    if (e.target.classList.contains('decline-request-btn') || e.target.closest('.decline-request-btn')) {
      const button = e.target.classList.contains('decline-request-btn') ? e.target : e.target.closest('.decline-request-btn');
      const username = button.getAttribute('data-username');
      respondToFriendRequest(username, 'reject', button);
    }
  });
  
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
        showNotification(`Friend request sent to ${username}!`, 'success');
      } else {
        showNotification(data.message || 'Failed to send friend request', 'error');
      }
    })
    .catch(error => {
      console.error('Error sending friend request:', error);
      showNotification('Failed to send friend request', 'error');
    });
  }
  
  // Remove friend function
  function removeFriend(username, button) {
    if (confirm(`Are you sure you want to remove ${username} from your friends?`)) {
      fetch('/api/friends/remove', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json'
        },
        body: JSON.stringify({
          friend_username: username
        })
      })
      .then(response => response.json())
      .then(data => {
        if (data.success) {
          const friendCard = button.closest('.friend-card');
          friendCard.style.opacity = '0';
          setTimeout(() => friendCard.remove(), 300);
          showNotification(`${username} removed from friends`, 'info');
        } else {
          showNotification(data.message || 'Failed to remove friend', 'error');
        }
      })
      .catch(error => {
        console.error('Error removing friend:', error);
        showNotification('Failed to remove friend', 'error');
      });
    }
  }
  
  // Respond to friend request function
  function respondToFriendRequest(username, action, button) {
    fetch('/api/friends/respond', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json'
      },
      body: JSON.stringify({
        requester_username: username,
        action: action
      })
    })
    .then(response => response.json())
    .then(data => {
      if (data.success) {
        const requestCard = button.closest('.request-card');
        if (action === 'accept') {
          button.innerHTML = '<i class="fas fa-check"></i> Accepted';
          button.style.background = '#42b883';
          showNotification(`You are now friends with ${username}!`, 'success');
          // Reload friends list to show new friend
          loadFriends();
        } else {
          button.innerHTML = '<i class="fas fa-times"></i> Declined';
          button.style.background = '#e74c3c';
          showNotification(`Friend request from ${username} declined`, 'info');
        }
        button.disabled = true;
        updateRequestCount(-1);
        setTimeout(() => {
          requestCard.style.opacity = '0';
          setTimeout(() => requestCard.remove(), 300);
        }, 1000);
      } else {
        showNotification(data.message || 'Failed to process friend request', 'error');
      }
    })
    .catch(error => {
      console.error('Error responding to friend request:', error);
      showNotification('Failed to process friend request', 'error');
    });
  }

  // Filter friends functionality
  if (filterSelect) {
    filterSelect.addEventListener("change", function () {
      const filterValue = this.value;
      const friendCards = document.querySelectorAll(".friend-card");

      friendCards.forEach((card) => {
        const onlineStatus = card.querySelector(".online-status");
        const isOnline =
          onlineStatus && onlineStatus.classList.contains("online");

        switch (filterValue) {
          case "online":
            card.style.display = isOnline ? "block" : "none";
            break;
          case "all":
          default:
            card.style.display = "block";
            break;
        }
      });
    });
  }

  // Friend request actions
  const acceptRequestButtons = document.querySelectorAll(".accept-request-btn");
  const declineRequestButtons = document.querySelectorAll(
    ".decline-request-btn"
  );
  const cancelRequestButtons = document.querySelectorAll(".cancel-request-btn");

  acceptRequestButtons.forEach((button) => {
    button.addEventListener("click", function () {
      const requestCard = this.closest(".request-card");
      const requestInfo = requestCard.querySelector("h4").textContent;

      // Animate the acceptance
      this.innerHTML = '<i class="fas fa-check"></i> Accepted';
      this.style.background = "#42b883";
      this.disabled = true;

      // Hide decline button
      const declineBtn = requestCard.querySelector(".decline-request-btn");
      if (declineBtn) {
        declineBtn.style.display = "none";
      }

      // Update badge count
      updateRequestCount(-1);

      // Remove card after animation
      setTimeout(() => {
        requestCard.style.opacity = "0";
        requestCard.style.transform = "translateY(-20px)";

        setTimeout(() => {
          requestCard.remove();
        }, 300);
      }, 1000);

      // Show notification
      showNotification(`You are now friends with ${requestInfo}!`, "success");
    });
  });

  declineRequestButtons.forEach((button) => {
    button.addEventListener("click", function () {
      const requestCard = this.closest(".request-card");
      const requestInfo = requestCard.querySelector("h4").textContent;

      // Animate the decline
      this.innerHTML = '<i class="fas fa-times"></i> Declined';
      this.style.background = "#e74c3c";
      this.disabled = true;

      // Hide accept button
      const acceptBtn = requestCard.querySelector(".accept-request-btn");
      if (acceptBtn) {
        acceptBtn.style.display = "none";
      }

      // Update badge count
      updateRequestCount(-1);

      // Remove card after animation
      setTimeout(() => {
        requestCard.style.opacity = "0";
        requestCard.style.transform = "translateY(-20px)";

        setTimeout(() => {
          requestCard.remove();
        }, 300);
      }, 1000);

      // Show notification
      showNotification(`Friend request from ${requestInfo} declined.`, "info");
    });
  });

  cancelRequestButtons.forEach((button) => {
    button.addEventListener("click", function () {
      const requestCard = this.closest(".request-card");
      const requestInfo = requestCard.querySelector("h4").textContent;

      // Animate the cancellation
      this.innerHTML = '<i class="fas fa-check"></i> Cancelled';
      this.disabled = true;

      // Remove card after animation
      setTimeout(() => {
        requestCard.style.opacity = "0";
        requestCard.style.transform = "translateY(-20px)";

        setTimeout(() => {
          requestCard.remove();
        }, 300);
      }, 1000);

      // Show notification
      showNotification(`Friend request to ${requestInfo} cancelled.`, "info");
    });
  });

  // Add friend suggestions
  const addFriendSuggestionButtons = document.querySelectorAll(
    ".add-friend-suggestion-btn"
  );
  const removeSuggestionButtons = document.querySelectorAll(
    ".remove-suggestion-btn"
  );

  addFriendSuggestionButtons.forEach((button) => {
    button.addEventListener("click", function () {
      const suggestionCard = this.closest(".suggestion-card");
      const suggestionInfo = suggestionCard.querySelector("h4").textContent;

      this.innerHTML = '<i class="fas fa-check"></i> Request Sent';
      this.style.background = "#42b883";
      this.disabled = true;

      // Show notification
      showNotification(`Friend request sent to ${suggestionInfo}!`, "success");
    });
  });

  removeSuggestionButtons.forEach((button) => {
    button.addEventListener("click", function () {
      const suggestionCard = this.closest(".suggestion-card");

      suggestionCard.style.opacity = "0";
      suggestionCard.style.transform = "scale(0.8)";

      setTimeout(() => {
        suggestionCard.remove();
      }, 300);
    });
  });

  // Message button functionality
  const messageButtons = document.querySelectorAll(".message-btn");
  messageButtons.forEach((button) => {
    button.addEventListener("click", function () {
      const friendCard = this.closest(".friend-card");
      const friendName = friendCard.querySelector("h3").textContent;

      // Simulate opening a message
      showNotification(`Opening chat with ${friendName}...`, "info");
    });
  });

  // Advanced search functionality
  const searchFriendsBtn = document.querySelector(".search-friends-btn");
  const searchFilterInputs = document.querySelectorAll(".search-filter-input");

  if (searchFriendsBtn) {
    searchFriendsBtn.addEventListener("click", function () {
      const searchValues = {};

      searchFilterInputs.forEach((input, index) => {
        const labels = ["name", "location", "school", "workplace"];
        searchValues[labels[index]] = input.value.trim();
      });

      // Simulate search
      this.innerHTML = '<i class="fas fa-spinner fa-spin"></i> Searching...';
      this.disabled = true;

      setTimeout(() => {
        this.innerHTML = '<i class="fas fa-search"></i> Search Friends';
        this.disabled = false;

        // Show mock results
        const resultsGrid = document.querySelector(".search-results-grid");
        if (
          resultsGrid &&
          Object.values(searchValues).some((val) => val !== "")
        ) {
          resultsGrid.style.display = "grid";
          showNotification(
            "Search completed! Found 2 potential friends.",
            "success"
          );
        } else {
          showNotification("Please enter search criteria.", "warning");
        }
      }, 2000);
    });
  }

  // Add friend from search results
  const addFriendResultButtons = document.querySelectorAll(
    ".add-friend-result-btn"
  );
  addFriendResultButtons.forEach((button) => {
    button.addEventListener("click", function () {
      const resultCard = this.closest(".search-result-card");
      const resultInfo = resultCard.querySelector("h5").textContent;

      this.innerHTML = '<i class="fas fa-check"></i> Request Sent';
      this.style.background = "#42b883";
      this.disabled = true;

      showNotification(`Friend request sent to ${resultInfo}!`, "success");
    });
  });

  // Helper functions
  function updateRequestCount(change) {
    const badge = document.querySelector(".tab-badge");
    if (badge) {
      const currentCount = parseInt(badge.textContent);
      const newCount = Math.max(0, currentCount + change);
      badge.textContent = newCount;

      if (newCount === 0) {
        badge.style.display = "none";
      }
    }

    // Update navbar notification badge
    const navBadge = document.querySelector(".notification-badge");
    if (navBadge) {
      const currentNavCount = parseInt(navBadge.textContent);
      const newNavCount = Math.max(0, currentNavCount + change);
      navBadge.textContent = newNavCount;

      if (newNavCount === 0) {
        navBadge.style.display = "none";
      }
    }
  }

  function showNotification(message, type = "info") {
    // Create notification element
    const notification = document.createElement("div");
    notification.className = `notification notification-${type}`;
    notification.innerHTML = `
            <i class="fas fa-${getNotificationIcon(type)}"></i>
            <span>${message}</span>
            <button class="notification-close">&times;</button>
        `;

    // Style the notification
    Object.assign(notification.style, {
      position: "fixed",
      top: "80px",
      right: "20px",
      background: getNotificationColor(type),
      color: "white",
      padding: "15px 20px",
      borderRadius: "8px",
      boxShadow: "0 4px 12px rgba(0, 0, 0, 0.2)",
      zIndex: "9999",
      display: "flex",
      alignItems: "center",
      gap: "10px",
      minWidth: "300px",
      transform: "translateX(100%)",
      transition: "transform 0.3s ease",
    });

    // Add to page
    document.body.appendChild(notification);

    // Animate in
    setTimeout(() => {
      notification.style.transform = "translateX(0)";
    }, 100);

    // Close button functionality
    const closeBtn = notification.querySelector(".notification-close");
    closeBtn.style.cssText = `
            background: none;
            border: none;
            color: white;
            font-size: 18px;
            cursor: pointer;
            margin-left: auto;
        `;

    closeBtn.addEventListener("click", () => {
      removeNotification(notification);
    });

    // Auto remove after 5 seconds
    setTimeout(() => {
      removeNotification(notification);
    }, 5000);
  }

  function removeNotification(notification) {
    notification.style.transform = "translateX(100%)";
    setTimeout(() => {
      if (notification.parentNode) {
        notification.parentNode.removeChild(notification);
      }
    }, 300);
  }

  function getNotificationIcon(type) {
    switch (type) {
      case "success":
        return "check-circle";
      case "warning":
        return "exclamation-triangle";
      case "error":
        return "times-circle";
      default:
        return "info-circle";
    }
  }

  function getNotificationColor(type) {
    switch (type) {
      case "success":
        return "#42b883";
      case "warning":
        return "#f39c12";
      case "error":
        return "#e74c3c";
      default:
        return "#0e4bf1";
    }
  }

  // Navigation hover effects
  // Create friend request card
  function createFriendRequestCard(request) {
    const card = document.createElement('div');
    card.className = 'request-card';
    card.innerHTML = `
      <div class="request-avatar">
        <img src="${request.profile_pic || 'https://via.placeholder.com/80'}" alt="${request.username}" />
      </div>
      <div class="request-info">
        <h4>${request.username}</h4>
        <p class="request-mutual">Wants to be your friend</p>
        <p class="request-time">Just now</p>
      </div>
      <div class="request-actions">
        <button class="accept-request-btn" onclick="acceptFriendRequest('${request.username}', this)">
          <i class="fas fa-check"></i> Accept
        </button>
        <button class="decline-request-btn" onclick="declineFriendRequest('${request.username}', this)">
          <i class="fas fa-times"></i> Decline
        </button>
      </div>
    `;
    return card;
  }

  // Accept friend request function
  window.acceptFriendRequest = function(username, button) {
    fetch('/api/friends/respond', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json'
      },
      body: JSON.stringify({
        requester_username: username,
        action: 'accept'
      })
    })
    .then(response => response.json())
    .then(data => {
      if (data.success) {
        button.innerHTML = '<i class="fas fa-check"></i> Accepted';
        button.style.background = '#42b883';
        button.disabled = true;
        
        // Hide decline button
        const declineBtn = button.parentElement.querySelector('.decline-request-btn');
        if (declineBtn) declineBtn.style.display = 'none';
        
        // Remove card after animation
        setTimeout(() => {
          const card = button.closest('.request-card');
          card.style.opacity = '0';
          card.style.transform = 'translateY(-20px)';
          setTimeout(() => card.remove(), 300);
        }, 1500);
        
        showNotification(`You are now friends with ${username}!`, 'success');
        // Reload friends list to show new friend
        loadFriends();
        // Update badge count
        updateRequestsBadge(-1);
      } else {
        showNotification(data.message || 'Failed to accept friend request', 'error');
      }
    })
    .catch(error => {
      console.error('Error accepting friend request:', error);
      showNotification('Failed to accept friend request', 'error');
    });
  };

  // Decline friend request function
  window.declineFriendRequest = function(username, button) {
    fetch('/api/friends/respond', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json'
      },
      body: JSON.stringify({
        requester_username: username,
        action: 'reject'
      })
    })
    .then(response => response.json())
    .then(data => {
      if (data.success) {
        button.innerHTML = '<i class="fas fa-times"></i> Declined';
        button.style.background = '#e74c3c';
        button.disabled = true;
        
        // Hide accept button
        const acceptBtn = button.parentElement.querySelector('.accept-request-btn');
        if (acceptBtn) acceptBtn.style.display = 'none';
        
        // Remove card after animation
        setTimeout(() => {
          const card = button.closest('.request-card');
          card.style.opacity = '0';
          card.style.transform = 'translateY(-20px)';
          setTimeout(() => card.remove(), 300);
        }, 1500);
        
        showNotification(`Friend request from ${username} declined`, 'info');
        // Update badge count
        updateRequestsBadge(-1);
      } else {
        showNotification(data.message || 'Failed to decline friend request', 'error');
      }
    })
    .catch(error => {
      console.error('Error declining friend request:', error);
      showNotification('Failed to decline friend request', 'error');
    });
  };

  // Update requests badge count
  function updateRequestsBadge(change) {
    const badge = document.querySelector('.tab-btn[data-tab="friend-requests"] .tab-badge');
    if (badge) {
      const currentCount = parseInt(badge.textContent) || 0;
      const newCount = typeof change === 'number' ? Math.max(0, currentCount + change) : change;
      badge.textContent = newCount;
      badge.style.display = newCount > 0 ? 'inline' : 'none';
    }
  }

  // Icon hover animations
  const navIcons = document.querySelectorAll(".nav-icon");
  navIcons.forEach((icon) => {
    icon.addEventListener("mouseenter", function () {
      this.style.transform = "scale(1.1)";
    });

    icon.addEventListener("mouseleave", function () {
      this.style.transform = "scale(1)";
    });
  });

  // Card hover animations
  const cards = document.querySelectorAll(
    ".friend-card, .request-card, .suggestion-card, .search-result-card"
  );
  cards.forEach((card) => {
    card.addEventListener("mouseenter", function () {
      this.style.transform = "translateY(-3px)";
    });

    card.addEventListener("mouseleave", function () {
      this.style.transform = "translateY(0)";
    });
  });
});

// Load current user info
function loadCurrentUser() {
  // Get username from session or API call
  fetch('/api/user/current')
    .then(response => response.json())
    .then(data => {
      if (data.username) {
        document.getElementById('current-username').textContent = data.username;
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

// Back navigation function
function goBack() {
  // Check if there's a previous page in history
  if (document.referrer && document.referrer.includes('/timeline')) {
    window.history.back();
  } else {
    // Default to timeline if no referrer or not from timeline
    window.location.href = '/timeline';
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
